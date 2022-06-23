#include "vtkPointPickCallback.h"
#include "transfer.h"

double* world_position_render1;  // 这是用来保存坐标的全局变量
double* world_position_render2;
double img_pos_render1[4];
double img_pos_render2[4];
int i, j = 0;

void vtkPointPickCallback::Execute(vtkObject* caller, unsigned long event, void* callData)
{
    if(vtkCommand::LeftButtonPressEvent)
    {
        //获取屏幕坐标
        int event_position[2];
        interactor->GetEventPosition(event_position);
        std::cout << "even position:" << event_position[0] << "," << event_position[1]<< std::endl;


        int* window_size = this->interactor->GetRenderWindow()->GetSize();
        std::cout << "window size:" << window_size[0] << "," << window_size[1]<< std::endl;
        
        //通过光线投影法获取当前坐标
        if(event_position[0]<0.5*window_size[0])
        {
            interactor->GetPicker()->Pick(event_position[0],event_position[1],0,
                                        renderer1);
            world_position_render1 = interactor->GetPicker()->GetPickPosition();
            img_pos_render1[i] = world_position_render1[0];
            img_pos_render1[i + 1] = world_position_render1[1];
            i += 2;
            std::cout << "world position render1:" << world_position_render1[0] << "," << world_position_render1[1] << std::endl;
        }
        else
        {
            interactor->GetPicker()->Pick(event_position[0],event_position[1],0,
                                        renderer2);
            world_position_render2 = interactor->GetPicker()->GetPickPosition();
            img_pos_render2[j] = world_position_render2[0];
            img_pos_render2[j + 1] = world_position_render2[1];
            j += 2;
            std::cout << "world position render2:" << world_position_render2[0] << "," << world_position_render2[1] << std::endl;
        }

    }

    
    if ((i==4)and(j==4))  //人工确定需要区域生长的起始点和终点，总共两张图个两个坐标共四个值
    {
        uchar threshhold[2] = { 0, 210 };
        std::cout << "pos render1: " << "(" << img_pos_render1[0] << " ," << img_pos_render1[1] << "), " << "(" << img_pos_render1[2] << " ," << img_pos_render1[3] << ")" <<std::endl;
        std::cout << "pos render2: " << "(" << img_pos_render2[0] << " ," << img_pos_render2[1] << "), " << "(" << img_pos_render2[2] << " ," << img_pos_render2[3] << ")" << std::endl;
        //使用区域动态生长进行图像分割
        img1 = RegionGrow(img1, img_pos_render1, threshhold);
        img2 = RegionGrow(img2, img_pos_render2, threshhold);
    }

    vtkSmartPointer<vtkImageData> vtk_img1;
    vtk_img1 = convertCVMatToVtkImageData(img1, true);
    actor1->SetInputData(vtk_img1);

    vtkSmartPointer<vtkImageData> vtk_img2;
    vtk_img2 = convertCVMatToVtkImageData(img2, true);
    actor2->SetInputData(vtk_img2);
    renderer1->Render();
}

/*
* cv::Mat RegionGrow(cv::Mat img, double img_pos_render[4], uchar SeedThreshold[2])
* param：
*       Mat: opencv格式的图像
*       img_pos_render[4]：数组，用来保存区域生长的起始点和终点，4个值分别是起点(x,y)、终点(x,y)
*       SeedThreshold[2]：数组，用来存储区域生长判断条件的上下限
* return：Mat,区域生长之后分割出来的部分
*/

cv::Mat RegionGrow(cv::Mat img, double img_pos_render[4], uchar SeedThreshold[2])
{
    //动态生长的起始点和终点
    int start_seed_x = img_pos_render[0];
    int start_seed_y = 512.0 - img_pos_render[1];
    int end_seed_x = img_pos_render[2];
    int end_seed_y = 512.0 - img_pos_render[3];
    cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);  //转化成灰度图片
    cv::Point start_seed = cv::Point(start_seed_x, start_seed_y);
    cv::Point end_seed = cv::Point(end_seed_x, end_seed_y);
    float value_1 = float(img.at<uchar>(cv::Point(start_seed.x, start_seed.y)));
    float value_2 = float(img.at<uchar>(cv::Point(end_seed.x, end_seed.y)));
    std::cout << "value_1:" << value_1 << endl;
    std::cout << "value_2:" << value_2 << endl;

    //计算起点和终点的均值作为阈值，和后面设定的阈值再做均值从而确定整个区域生长的阈值
    float threshould = (value_1 + value_2)/2.0;
    std::cout << "threshold:" << threshould << endl;

    cv::Mat growArea = cv::Mat::ones(img.size(), CV_8UC1) * 255;  //生长区域
    growArea.at<uchar>(cv::Point(start_seed.x, start_seed.y)) = img.at<uchar>(cv::Point(start_seed.x, start_seed.y));
    cv::Point waitSeed;  //待生长种子点
    int waitSeed_value = 0;  //待生长种子点像素值
    int opp_waitSeed_value = 0;  //mat_thresh中对应的待生长种子点出的像素值
    std::vector<cv::Point> SeedVector;  //种子队列
    SeedVector.push_back(start_seed);  //压入种子点
    int direct[4][2] = { {0,1}, {0,-1}, {1,0}, {-1,0} };  //确定方向，这里使用4邻域方向

    while (!SeedVector.empty())
    {
        cv::Point seed = SeedVector.back();  //取出一个点，注意这里是先进先出
        SeedVector.pop_back();
        for (int i = 0; i <= 3; i++)
        {
            waitSeed.x = seed.x + direct[i][0];
            waitSeed.y = seed.y + direct[i][1];
            
            //排出边缘点
            if (waitSeed.x < 0 || waitSeed.y < 0 ||
                waitSeed.x >(img.cols - 1) || waitSeed.y >(img.rows - 1))
                continue;
            
            //取生长区域的图像点以及原图对应的(waitSeed.x, waitSeed.y)点
            waitSeed_value = growArea.at<uchar>(cv::Point(waitSeed.x, waitSeed.y));
            opp_waitSeed_value = img.at<uchar>(cv::Point(waitSeed.x, waitSeed.y));
            //std::cout << "opp_waitSeed_value:" << opp_waitSeed_value << endl;

            if (waitSeed_value == 255)
            {   
                //使用设定的阈值的上限和起点终点的均值再做均值，从而作为上限
                if ( (SeedThreshold[0] <= opp_waitSeed_value) and (opp_waitSeed_value <= int((float(SeedThreshold[1]) + threshould)) / 2.0) ) //生长条件，小于一定的阈值就放进来
                {
                    growArea.at<uchar>(cv::Point(waitSeed.x, waitSeed.y)) = opp_waitSeed_value;
                    //std::cout << "value:" << opp_waitSeed_value << endl;
                    SeedVector.push_back(waitSeed);  //放入新的种子点
                    // 如果取值到了终点，那就停止
                    if ((waitSeed.x == end_seed.x) and (waitSeed.y == end_seed.y))
                        return growArea;   
                }
            }
        }
    }
    return growArea;
}