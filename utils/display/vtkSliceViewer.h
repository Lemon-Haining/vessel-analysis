#ifndef VTK_SLICE_VIEWER_H
#define VTK_SLICE_VIEWER_H

#include "vtkViewer.h"
#include "vtkPointPicker.h"

/********************************
类名：vtkSliceViewer
作者：赵海宁
功能：
    |-基础功能
        |-根据输入的三维图像数据，切平面坐标系（三个坐标轴方向，原点坐标），显示切平面位置图像
    |-拓展功能
        |-显示其他actor
        |-设置InteractorStyle
        |-添加observer
        |-设置PointPicker
示例：
    double normal[16] = {
        1,0,0,0,
        0,0.866025,-0.5,0,
        0,0.5,0.866025,0,
        0,0,0,1
    };

    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetPosition(0,0,0);
    axes->SetTotalLength(300,300,300);
    axes->SetAxisLabels(0);


    vtkSliceViewer* viewer = new vtkSliceViewer;
    //设置三维数据，切平面坐标系，法向间隔
    viewer->SetInputData(vtk_volume);
    viewer->SetNormal(normal);
    viewer->SetNormalSpace(0.6);
    viewer->Init();
    
    //设置切片位置
    viewer->SetSlice(40);

    //设置显示窗口、交互风格、PointPicker、observer、其他actor
    viewer->SetRenderWindow(window);
    viewer->SetInteractorStyle(style);
    viewer->SetPointPicker();
    vtkSmartPointer<vtkPointPickCallback> pick_callback = vtkSmartPointer<vtkPointPickCallback>::New();
    pick_callback->SetInteractor(viewer->GetInteractor());
    viewer->AddObserver(pick_callback);
    viewer->AddActor(axes);

    //启动渲染管线和交互器
    viewer->Start();
    delete viewer;
********************************/

class vtkSliceViewer : public vtkViewer
{
    public:
    vtkSliceViewer()
    {
        point_picker = vtkSmartPointer<vtkPointPicker>::New();
        slice = vtkSmartPointer<vtkImageData>::New();
        main_actor = vtkSmartPointer<vtkImageActor>::New();
    }
    ~vtkSliceViewer(){}
    
    /**
     * @brief 重写vtkViewer的虚函数
     * 设置main_actor的数据来源、位姿等信息
     * 将main_actor添加到renderer中
     */
    virtual void CreateMainActor();

    /**
     * @brief 设置交互器interactor的picker类型
     * 
     */
    void SetPointPicker();

    /**
     * @brief 设置初始坐标系矩阵
     * 
     * @param basis 坐标系矩阵，4X4类型，前3X3矩阵为切片坐标系x轴，y轴，z轴的方向向量，第四个列向量为切片坐标系原点坐标
     */
    void SetBasis(double* basis);

    /**
     * @brief 设置切片间隔
     * 
     * @param normal_space 切片间隔大小 
     */
    void SetNormalSpace(double normal_space);
    
    /**
     * @brief 计算输入数据的相关信息：
     * space[3]: 三个方向上的像素间隔
     * extent[6]: 三个方向上的索引范围
     * origin[3]: 图像起点坐标
     * center[3]: 图像中心坐标
     * max: 切片索引最大值
     * min: 切片索引最小值
     */
    void InitDataInfo();

    /**
     * @brief 设置切片索引，主要是通过设置原点坐标得到
     * 
     * @param slice_number 切片索引
     */
    void SetSlice(int slice_number);

    int GetSliceMax();
    int GetSliceMin();

    private:
    //外部设置
    double* basis;
    double position[3];
    double normal_space;

    //内部声明
    vtkSmartPointer<vtkPointPicker> point_picker;
    vtkSmartPointer<vtkImageData> slice;
    vtkSmartPointer<vtkImageActor> main_actor;
    
    //volume的信息
    double space[3];//像素间隔
    double origin[3];//图像索引起点[0,0,0]的物理世界坐标
    int extent[6];
    double center[3];//图像中心的物理世界坐标
    int max;
    int min;
};

#endif