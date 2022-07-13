/*-----Editored by HuangWang-2017年2月21日-----*/
/*-----功能描述：读取手工匹配的两幅冠脉造影的二维匹配点坐标,*/
/*-----利用冠脉造影算法进行三维点重建，并在VTK中使用大小不一的圆柱管道进行重建-----*/
/*-----程序员专用分割线-----*/

//包含C++标准输入输出头文件，用到的OpenCV类以及VTK类的头文件
#include "vtkManual3DReconstruction.h"
using namespace cv; //使用OpenCV命名空间
using namespace std;//使用C++命名空间

#define  PI 3.14159265358979323846 //宏定义圆周率PI

void InitMat(Mat& m, double* num)//用一维数组初始化OpenCV矩阵Mat对象
{
	for (int i = 0; i < m.rows; i++)
	{
		for (int j = 0; j < m.cols; j++)
		{
			m.at<double>(i, j) = *(num + i * m.cols + j);
		}
	}
}

Mat GenerateRymat(double jiaodu)//构造Ry矩阵OpenCV矩阵Mat对象,参数角度为角度，直接返回一个Mat对象
{
	double m[] = { cos(jiaodu / 180 * PI),0,-sin(jiaodu / 180 * PI),
		0,1,0,
		sin(jiaodu / 180 * PI),0,cos(jiaodu / 180 * PI)
	};
	Mat Mtemp(3, 3, CV_64F);
	InitMat(Mtemp, m);
	return Mtemp;
}

Mat GenerateRxmat(double jiaodu)//构造Rx矩阵OpenCV矩阵Mat对象,参数角度为角度,直接返回一个Mat对象
{
	double m[] = { 1,0,0,
		0,cos(jiaodu / 180 * PI),sin(jiaodu / 180 * PI),
		0,-sin(jiaodu / 180 * PI),cos(jiaodu / 180 * PI)
	};
	Mat Mtemp(3, 3, CV_64F);
	InitMat(Mtemp, m);
	return Mtemp;
}

float ExtractMatIJValue(Mat& M, int i, int j)//提取Mat对象第i行第j列的数值,M为Mat对象，ij为行列值
{
	return M.at<double>(i, j);
}



Mat Generate3DAmat(Mat& M, My_Scalefactor& myfactor)//生成最后求三维点的A矩阵，M为Mat类对象,为先求出的R矩阵，My_Scalefactor为struct
{
	Mat Mattemp(4, 3, CV_64F);
	Mattemp.at<double>(0, 0) = 1;
	Mattemp.at<double>(0, 1) = 0.0;
	Mattemp.at<double>(0, 2) = -myfactor.IamgeAXfactor;
	Mattemp.at<double>(1, 0) = 0;
	Mattemp.at<double>(1, 1) = 1;
	Mattemp.at<double>(1, 2) = -myfactor.ImageAYfactor;
	Mattemp.at<double>(2, 0) = ExtractMatIJValue(M, 0, 0) - (ExtractMatIJValue(M, 2, 0)

		* myfactor.ImageBXfactor);
	Mattemp.at<double>(2, 1) = ExtractMatIJValue(M, 0, 1) - (ExtractMatIJValue(M, 2, 1)

		* myfactor.ImageBXfactor);
	Mattemp.at<double>(2, 2) = ExtractMatIJValue(M, 0, 2) - (ExtractMatIJValue(M, 2, 2)

		* myfactor.ImageBXfactor);
	Mattemp.at<double>(3, 0) = ExtractMatIJValue(M, 1, 0) - (ExtractMatIJValue(M, 2, 0)

		* myfactor.ImageBYfactor);
	Mattemp.at<double>(3, 1) = ExtractMatIJValue(M, 1, 1) - (ExtractMatIJValue(M, 2, 1)

		* myfactor.ImageBYfactor);
	Mattemp.at<double>(3, 2) = ExtractMatIJValue(M, 1, 2) - (ExtractMatIJValue(M, 2, 2)

		* myfactor.ImageBYfactor);
	//	InitMat(Mattemp,m);
	return Mattemp;

}

Mat Genreate3Damat(Mat& M, My_Scalefactor& myfactor)//生成求B矩阵的a矩阵
{
	double m[] = { ExtractMatIJValue(M,0,0) - ExtractMatIJValue(M,2,0) * myfactor.ImageBXfactor,
		ExtractMatIJValue(M,0,1) - ExtractMatIJValue(M,2,1) * myfactor.ImageBXfactor,
		ExtractMatIJValue(M,0,2) - ExtractMatIJValue(M,2,2) * myfactor.ImageBXfactor
	};
	Mat Mattemp(1, 3, CV_64F);
	InitMat(Mattemp, m);
	return Mattemp;
}

Mat Generate3DTmat(double T)//生成求B矩阵的T矩阵
{

	Mat Mattemp(3, 1, CV_64F);
	Mattemp.at<double>(0, 0) = 0.0;
	Mattemp.at<double>(1, 0) = 0.0;
	Mattemp.at<double>(2, 0) = T;

	return Mattemp;
}

Mat Generate3Dbmat(Mat& M, My_Scalefactor& myfactor)//生成求B矩阵的b矩阵
{
	double m[] = { ExtractMatIJValue(M,1,0) - ExtractMatIJValue(M,2,0) * myfactor.ImageBYfactor,
		ExtractMatIJValue(M,1,1) - ExtractMatIJValue(M,2,1) * myfactor.ImageBYfactor,
		ExtractMatIJValue(M,1,2) - ExtractMatIJValue(M,2,2) * myfactor.ImageBYfactor
	};
	Mat Mattemp(1, 3, CV_64F);
	InitMat(Mattemp, m);
	return Mattemp;
}

Mat Generate3DBmat(Mat& a, Mat& b, Mat& t)//生成最后求三维点的B矩阵
{
	Mat at, bt;
	at = a * t;
	bt = b * t;
	Mat Mattemp(4, 1, CV_64F);
	Mattemp.at<double>(0, 0) = 0.0;
	Mattemp.at<double>(1, 0) = 0.0;
	Mattemp.at<double>(2, 0) = at.at<double>(0, 0);
	Mattemp.at<double>(3, 0) = bt.at<double>(0, 0);
	return Mattemp;
}


int vtkManual3DReconstruction()
{
	Mat R;//定义旋转矩阵对象
	Mat t;//定义平移矩阵对象
	//	Mat Rxp2,Rya2,Rya1,Rxp1;//构建求旋转矩阵R的原始矩阵
	//ImageA角度参数为 RAO 0，CAUD 35 所以a1=0,p1=35
	//ImageB角度参数为 LAO 80，CRAN 15 所以a2=80,p2=15
	//因为a1为RAO角度，所以a1=-a1=0,p1=35，a2为LAO角度，a2=-a2=-80,
	//因为p2为CRAN角度，所以p2=-p2=-15
	double a1, a2, p1, p2;//声明角度
	/*a1=0;
	a2=-80;
	p1=35;
	p2=-15;*/

	//对角度进行赋值
	a1 = 0;
	a2 = 80;
	p1 = 35;
	p2 = 15;

	R = GenerateRxmat(p2) * GenerateRymat(a2) * GenerateRymat(a1) * GenerateRxmat(-p1);//求旋转矩阵R

	double T1 = 755, T2 = 865;//定义距离变量，T1表示O点到s1的距离，T2表示O点到s2的距离
	t = Generate3DTmat(T1) - (R.inv() * Generate3DTmat(T2));//求平移矩阵t


	vector<My2dPoint>ImageApoint;//声明存储ImageA轮廓线点坐标的容器
	ImageApoint.clear();//刚开始先清除容器中的数据
	vector<My2dPoint>ImageBpoint;//声明存储ImageB轮廓线点坐标的容器
	ImageBpoint.clear();//刚开始先清除容器中的数据


	/*-----从txt中读取两幅图片手工匹配的二维点数据，并将其填充到容器之中去-----*/
	FILE* ImageA = NULL;//定义读取图片A上二维点坐标的文件指针
	FILE* ImageB = NULL;//定义读取图片B上二维点坐标的文件指针

	ImageA = fopen("./ReconstructionData/ImageA2.txt", "rb");//打开存储图片A二维点坐标的txt文件
	if (!ImageA)
	{
		cout << "打开文件失败" << endl;
	}
	double ImageAx, ImageAy;//定义临时二维点坐标对象
	My2dPoint ImageAtemp;//定义临时的二维点结构体

	while (!feof(ImageA))//逐行读取数据
	{
		fscanf(ImageA, "%lf %lf", &ImageAx, &ImageAy);//按格式读取数据
		//将临时变量的值赋给二维结构体的相应对象
		ImageAtemp.x = ImageAx;
		ImageAtemp.y = ImageAy;
		ImageApoint.push_back(ImageAtemp);//向容器中压入这个结构体
	}

	ImageB = fopen("./ReconstructionData/ImageB2.txt", "rb");//打开存储图片B二维点坐标的txt文件
	if (!ImageB)
	{
		cout << "打开文件失败" << endl;
	}
	double ImageBx, ImageBy;//定义临时二维点坐标对象
	My2dPoint ImageBtemp;//定义临时的二维点结构体
	while (!feof(ImageB))//逐行读取数据
	{
		fscanf(ImageB, "%lf %lf", &ImageBx, &ImageBy);//按格式读取数据
		//将临时变量的值赋给二维结构体的相应对象
		ImageBtemp.x = ImageBx;
		ImageBtemp.y = ImageBy;
		ImageBpoint.push_back(ImageBtemp);//向容器中压入这个结构体
	}


	//定义X射线源S1和S2到各自投影平面的垂直距离D1，D2
	int D1 = 987, D2 = 987;

	vector<My3dPoint> Result3Dpoint;//声明存储最后由ImageA和ImageB二维点所求的三维点坐标的容器
	Result3Dpoint.clear();
	for (int i = 0; i < ImageApoint.size() - 1; i++)
	{
		/*-----求解缩放因子-----*/
		My_Scalefactor temp;
		temp.IamgeAXfactor = ImageApoint[i].x / D1;
		temp.ImageAYfactor = ImageApoint[i].y / D1;
		temp.ImageBXfactor = ImageBpoint[i].x / D2;
		temp.ImageBYfactor = ImageBpoint[i].y / D2;

		/*-----实现从二维点向三维点转换-----*/
		Mat A, a, b, B;//声明进行最后三维点计算的矩阵，以及求解
		A = Generate3DAmat(R, temp);
		a = Genreate3Damat(R, temp);
		b = Generate3Dbmat(R, temp);
		B = Generate3DBmat(a, b, t);

		Mat C;//声明三维点坐标的结果矩阵
		C = (A.t() * A).inv() * A.t() * B;
		My3dPoint Result;
		Result.x = C.at<double>(0);
		Result.y = C.at<double>(1);
		Result.z = C.at<double>(2);
		ofstream f;
		f.open("./ReconstructionData/3DPoints.txt", ios::out | ios::app);
		f << Result.x << "	" << Result.y << "	" << Result.z << endl;
		Result3Dpoint.push_back(Result);
	}

	/*-----很据上述求解出来的冠脉三维点重建的结果，按顺序进行样条插值生成样条曲线-----*/
	/*-----并以插值后生成的样条曲线为中心生成圆柱体管道，通过设置不同的圆柱体半径达到模拟冠脉的目的-----*/
	/*-----定义vtk绘图命令,开始绘制-----*/

	/*-----绘制第一段样条插值曲线以及圆柱体管道，从0点到1点-----*/
	/*-----对一段圆柱体管道的绘制做详细注释，以下几段相似-----*/
	vtkSmartPointer<vtkPoints>point01 = vtkSmartPointer<vtkPoints>::New(); //声明第一段样条曲线中所包含的点的VTK智能指针
	//根据上述求得的三维点的结果，向第一段线的点的智能指针中插入三维点
	point01->InsertPoint(0, Result3Dpoint[0].x, Result3Dpoint[0].y, Result3Dpoint[0].z);
	point01->InsertPoint(1, Result3Dpoint[1].x, Result3Dpoint[1].y, Result3Dpoint[1].z);
	point01->InsertPoint(2, Result3Dpoint[2].x, Result3Dpoint[2].y, Result3Dpoint[2].z);
	point01->InsertPoint(3, Result3Dpoint[3].x, Result3Dpoint[3].y, Result3Dpoint[3].z);
	point01->InsertPoint(4, Result3Dpoint[4].x, Result3Dpoint[4].y, Result3Dpoint[4].z);

	/*-----根据三维点插值成样条曲线-----*/
	vtkSmartPointer<vtkParametricSpline>spline01 = vtkSmartPointer<vtkParametricSpline>::New();
	spline01->SetPoints(point01);
	spline01->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource01 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource01->SetParametricFunction(spline01);

	/*定义样条曲线的Mapper以及Actor*/
	vtkSmartPointer<vtkPolyDataMapper>splineMapper01 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper01->SetInputConnection(splineSource01->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor01 = vtkSmartPointer<vtkActor>::New();
	splineActor01->SetMapper(splineMapper01);
	splineActor01->GetProperty()->SetColor(0.38, 0.7, 0.16);//设置样条曲线的颜色

	/*以生成的样条曲线为中心生成圆柱体管道*/
	vtkSmartPointer<vtkTubeFilter>tube01 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube01->SetInput(splineSource01->GetOutput());
	tube01->SetInputConnection(splineSource01->GetOutputPort());//输入样条曲线
	tube01->SetNumberOfSides(20);//设置圆柱体管道的侧边的数量，数量越多越圆滑
	tube01->SetRadius(3.5);//设置圆柱体管道的半径

	/*设置生成的圆柱体管道的Mapper和Actor，以便在VTK中进行绘制*/
	vtkSmartPointer<vtkPolyDataMapper>tubeMapper01 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper01->SetInput(tube01->GetOutput());
	tubeMapper01->SetInputConnection(tube01->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor01 = vtkSmartPointer<vtkActor>::New();
	tubeActor01->SetMapper(tubeMapper01);
	tubeActor01->GetProperty()->SetColor(1, 0, 0);//设置圆柱体的颜色

	/*-----绘制第二段样条插值曲线以及圆柱体管道，从1点到2点-----*/
	vtkSmartPointer<vtkPoints>point12 = vtkSmartPointer<vtkPoints>::New();
	point12->InsertPoint(0, Result3Dpoint[4].x, Result3Dpoint[4].y, Result3Dpoint[4].z);
	point12->InsertPoint(1, Result3Dpoint[5].x, Result3Dpoint[5].y, Result3Dpoint[5].z);
	point12->InsertPoint(2, Result3Dpoint[6].x, Result3Dpoint[6].y, Result3Dpoint[6].z);
	point12->InsertPoint(3, Result3Dpoint[7].x, Result3Dpoint[7].y, Result3Dpoint[7].z);
	point12->InsertPoint(4, Result3Dpoint[8].x, Result3Dpoint[8].y, Result3Dpoint[8].z);
	point12->InsertPoint(5, Result3Dpoint[9].x, Result3Dpoint[9].y, Result3Dpoint[9].z);
	point12->InsertPoint(6, Result3Dpoint[10].x, Result3Dpoint[10].y, Result3Dpoint[10].z);

	vtkSmartPointer<vtkParametricSpline>spline12 = vtkSmartPointer<vtkParametricSpline>::New();
	spline12->SetPoints(point12);
	spline12->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource12 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource12->SetParametricFunction(spline12);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper12 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper12->SetInputConnection(splineSource12->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor12 = vtkSmartPointer<vtkActor>::New();
	splineActor12->SetMapper(splineMapper12);
	splineActor12->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube12 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube12->SetInput(splineSource12->GetOutput());
	tube12->SetInputConnection(splineSource12->GetOutputPort());
	tube12->SetNumberOfSides(20);
	tube12->SetRadius(3);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper12 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper12->SetInput(tube12->GetOutput());
	tubeMapper12->SetInputConnection(tube12->GetOutputPort());
	
	vtkSmartPointer<vtkActor>tubeActor12 = vtkSmartPointer<vtkActor>::New();
	tubeActor12->SetMapper(tubeMapper12);
	tubeActor12->GetProperty()->SetColor(1, 0, 0);


	/*-----绘制第三段样条插值曲线以及圆柱体管道，从2点到3点-----*/
	vtkSmartPointer<vtkPoints>point23 = vtkSmartPointer<vtkPoints>::New();
	point23->InsertPoint(0, Result3Dpoint[10].x, Result3Dpoint[10].y, Result3Dpoint[10].z);
	point23->InsertPoint(1, Result3Dpoint[11].x, Result3Dpoint[11].y, Result3Dpoint[11].z);
	point23->InsertPoint(2, Result3Dpoint[12].x, Result3Dpoint[12].y, Result3Dpoint[12].z);
	point23->InsertPoint(3, Result3Dpoint[13].x, Result3Dpoint[13].y, Result3Dpoint[13].z);
	point23->InsertPoint(4, Result3Dpoint[14].x, Result3Dpoint[14].y, Result3Dpoint[14].z);
	point23->InsertPoint(5, Result3Dpoint[15].x, Result3Dpoint[15].y, Result3Dpoint[15].z);


	vtkSmartPointer<vtkParametricSpline>spline23 = vtkSmartPointer<vtkParametricSpline>::New();
	spline23->SetPoints(point23);
	spline23->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource23 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource23->SetParametricFunction(spline23);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper23 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper23->SetInputConnection(splineSource23->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor23 = vtkSmartPointer<vtkActor>::New();
	splineActor23->SetMapper(splineMapper23);
	splineActor23->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube23 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube23->SetInput(splineSource23->GetOutput());
	tube23->SetInputConnection(splineSource23->GetOutputPort());
	tube23->SetNumberOfSides(23);
	tube23->SetRadius(2.5);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper23 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper23->SetInput(tube23->GetOutput());
	tubeMapper23->SetInputConnection(tube23->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor23 = vtkSmartPointer<vtkActor>::New();
	tubeActor23->SetMapper(tubeMapper23);
	tubeActor23->GetProperty()->SetColor(1, 0, 0);

	/*-----绘制第四段样条插值曲线以及圆柱体管道，从2点到4点-----*/
	vtkSmartPointer<vtkPoints>point24 = vtkSmartPointer<vtkPoints>::New();
	point24->InsertPoint(0, Result3Dpoint[10].x, Result3Dpoint[10].y, Result3Dpoint[10].z);
	point24->InsertPoint(1, Result3Dpoint[16].x, Result3Dpoint[16].y, Result3Dpoint[16].z);
	point24->InsertPoint(2, Result3Dpoint[17].x, Result3Dpoint[17].y, Result3Dpoint[17].z);
	point24->InsertPoint(3, Result3Dpoint[18].x, Result3Dpoint[18].y, Result3Dpoint[18].z);
	point24->InsertPoint(4, Result3Dpoint[19].x, Result3Dpoint[19].y, Result3Dpoint[19].z);
	point24->InsertPoint(5, Result3Dpoint[20].x, Result3Dpoint[20].y, Result3Dpoint[20].z);


	vtkSmartPointer<vtkParametricSpline>spline24 = vtkSmartPointer<vtkParametricSpline>::New();
	spline24->SetPoints(point24);
	spline24->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource24 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource24->SetParametricFunction(spline24);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper24 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper24->SetInputConnection(splineSource24->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor24 = vtkSmartPointer<vtkActor>::New();
	splineActor24->SetMapper(splineMapper24);
	splineActor24->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube24 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube24->SetInput(splineSource24->GetOutput());
	tube24->SetInputConnection(splineSource24->GetOutputPort());
	tube24->SetNumberOfSides(24);
	tube24->SetRadius(2.5);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper24 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper24->SetInput(tube24->GetOutput());
	tubeMapper24->SetInputConnection(tube24->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor24 = vtkSmartPointer<vtkActor>::New();
	tubeActor24->SetMapper(tubeMapper24);
	tubeActor24->GetProperty()->SetColor(1, 0, 0);

	/*-----绘制第五段样条插值曲线以及圆柱体管道，从4点到5点-----*/
	vtkSmartPointer<vtkPoints>point45 = vtkSmartPointer<vtkPoints>::New();
	point45->InsertPoint(0, Result3Dpoint[20].x, Result3Dpoint[20].y, Result3Dpoint[20].z);
	point45->InsertPoint(1, Result3Dpoint[21].x, Result3Dpoint[21].y, Result3Dpoint[21].z);
	point45->InsertPoint(2, Result3Dpoint[22].x, Result3Dpoint[22].y, Result3Dpoint[22].z);
	point45->InsertPoint(3, Result3Dpoint[23].x, Result3Dpoint[23].y, Result3Dpoint[23].z);
	point45->InsertPoint(4, Result3Dpoint[24].x, Result3Dpoint[24].y, Result3Dpoint[24].z);
	point45->InsertPoint(5, Result3Dpoint[25].x, Result3Dpoint[25].y, Result3Dpoint[25].z);


	vtkSmartPointer<vtkParametricSpline>spline45 = vtkSmartPointer<vtkParametricSpline>::New();
	spline45->SetPoints(point45);
	spline45->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource45 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource45->SetParametricFunction(spline45);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper45 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper45->SetInputConnection(splineSource45->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor45 = vtkSmartPointer<vtkActor>::New();
	splineActor45->SetMapper(splineMapper45);
	splineActor45->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube45 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube45->SetInput(splineSource45->GetOutput());
	tube45->SetInputConnection(splineSource45->GetOutputPort());
	tube45->SetNumberOfSides(24);
	tube45->SetRadius(2);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper45 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper45->SetInput(tube45->GetOutput());
	tubeMapper45->SetInputConnection(tube45->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor45 = vtkSmartPointer<vtkActor>::New();
	tubeActor45->SetMapper(tubeMapper45);
	tubeActor45->GetProperty()->SetColor(1, 0, 0);

	/*-----绘制第六段样条插值曲线以及圆柱体管道，从4点到6点-----*/
	vtkSmartPointer<vtkPoints>point46 = vtkSmartPointer<vtkPoints>::New();
	point46->InsertPoint(0, Result3Dpoint[20].x, Result3Dpoint[20].y, Result3Dpoint[20].z);
	point46->InsertPoint(1, Result3Dpoint[26].x, Result3Dpoint[26].y, Result3Dpoint[26].z);
	point46->InsertPoint(2, Result3Dpoint[27].x, Result3Dpoint[27].y, Result3Dpoint[27].z);
	point46->InsertPoint(3, Result3Dpoint[28].x, Result3Dpoint[28].y, Result3Dpoint[28].z);
	point46->InsertPoint(4, Result3Dpoint[29].x, Result3Dpoint[29].y, Result3Dpoint[29].z);


	vtkSmartPointer<vtkParametricSpline>spline46 = vtkSmartPointer<vtkParametricSpline>::New();
	spline46->SetPoints(point46);
	spline46->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource46 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource46->SetParametricFunction(spline46);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper46 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper46->SetInputConnection(splineSource46->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor46 = vtkSmartPointer<vtkActor>::New();
	splineActor46->SetMapper(splineMapper46);
	splineActor46->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube46 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube46->SetInput(splineSource46->GetOutput());
	tube46->SetInputConnection(splineSource46->GetOutputPort());
	tube46->SetNumberOfSides(24);
	tube46->SetRadius(1.5);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper46 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper46->SetInput(tube46->GetOutput());
	tubeMapper46->SetInputConnection(tube46->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor46 = vtkSmartPointer<vtkActor>::New();
	tubeActor46->SetMapper(tubeMapper46);
	tubeActor46->GetProperty()->SetColor(1, 0, 0);

	/*-----绘制第七段样条插值曲线以及圆柱体管道，从1点到7点-----*/
	vtkSmartPointer<vtkPoints>point17 = vtkSmartPointer<vtkPoints>::New();
	point17->InsertPoint(0, Result3Dpoint[4].x, Result3Dpoint[4].y, Result3Dpoint[4].z);
	point17->InsertPoint(1, Result3Dpoint[31].x, Result3Dpoint[31].y, Result3Dpoint[31].z);
	point17->InsertPoint(2, Result3Dpoint[32].x, Result3Dpoint[32].y, Result3Dpoint[32].z);
	point17->InsertPoint(3, Result3Dpoint[33].x, Result3Dpoint[33].y, Result3Dpoint[33].z);
	point17->InsertPoint(4, Result3Dpoint[34].x, Result3Dpoint[34].y, Result3Dpoint[34].z);
	point17->InsertPoint(4, Result3Dpoint[35].x, Result3Dpoint[35].y, Result3Dpoint[35].z);
	point17->InsertPoint(4, Result3Dpoint[36].x, Result3Dpoint[36].y, Result3Dpoint[36].z);
	point17->InsertPoint(4, Result3Dpoint[37].x, Result3Dpoint[37].y, Result3Dpoint[37].z);
	point17->InsertPoint(4, Result3Dpoint[38].x, Result3Dpoint[38].y, Result3Dpoint[38].z);



	vtkSmartPointer<vtkParametricSpline>spline17 = vtkSmartPointer<vtkParametricSpline>::New();
	spline17->SetPoints(point17);
	spline17->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource17 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource17->SetParametricFunction(spline17);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper17 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper17->SetInputConnection(splineSource17->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor17 = vtkSmartPointer<vtkActor>::New();
	splineActor17->SetMapper(splineMapper17);
	splineActor17->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube17 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube17->SetInput(splineSource17->GetOutput());
	tube17->SetInputConnection(splineSource17->GetOutputPort());
	tube17->SetNumberOfSides(17);
	tube17->SetRadius(3);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper17 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper17->SetInput(tube17->GetOutput());
	tubeMapper17->SetInputConnection(tube17->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor17 = vtkSmartPointer<vtkActor>::New();
	tubeActor17->SetMapper(tubeMapper17);
	tubeActor17->GetProperty()->SetColor(1, 0, 0);

	/*-----绘制第八段样条插值曲线以及圆柱体管道，从7点到8点-----*/
	vtkSmartPointer<vtkPoints>point78 = vtkSmartPointer<vtkPoints>::New();
	point78->InsertPoint(0, Result3Dpoint[38].x, Result3Dpoint[38].y, Result3Dpoint[38].z);
	point78->InsertPoint(1, Result3Dpoint[39].x, Result3Dpoint[39].y, Result3Dpoint[39].z);
	point78->InsertPoint(2, Result3Dpoint[40].x, Result3Dpoint[40].y, Result3Dpoint[40].z);
	point78->InsertPoint(3, Result3Dpoint[41].x, Result3Dpoint[41].y, Result3Dpoint[41].z);
	point78->InsertPoint(4, Result3Dpoint[42].x, Result3Dpoint[42].y, Result3Dpoint[42].z);
	point78->InsertPoint(4, Result3Dpoint[43].x, Result3Dpoint[43].y, Result3Dpoint[43].z);


	vtkSmartPointer<vtkParametricSpline>spline78 = vtkSmartPointer<vtkParametricSpline>::New();
	spline78->SetPoints(point78);
	spline78->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource78 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource78->SetParametricFunction(spline78);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper78 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper78->SetInputConnection(splineSource78->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor78 = vtkSmartPointer<vtkActor>::New();
	splineActor78->SetMapper(splineMapper78);
	splineActor78->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube78 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube78->SetInput(splineSource78->GetOutput());
	tube78->SetInputConnection(splineSource78->GetOutputPort());
	tube78->SetNumberOfSides(24);
	tube78->SetRadius(2.5);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper78 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper78->SetInput(tube78->GetOutput());
	tubeMapper78->SetInputConnection(tube78->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor78 = vtkSmartPointer<vtkActor>::New();
	tubeActor78->SetMapper(tubeMapper78);
	tubeActor78->GetProperty()->SetColor(1, 0, 0);

	/*-----绘制第九段样条插值曲线以及圆柱体管道，从7点到9点-----*/
	vtkSmartPointer<vtkPoints>point79 = vtkSmartPointer<vtkPoints>::New();
	point79->InsertPoint(0, Result3Dpoint[38].x, Result3Dpoint[38].y, Result3Dpoint[38].z);
	point79->InsertPoint(1, Result3Dpoint[44].x, Result3Dpoint[44].y, Result3Dpoint[44].z);
	point79->InsertPoint(2, Result3Dpoint[45].x, Result3Dpoint[45].y, Result3Dpoint[45].z);
	point79->InsertPoint(3, Result3Dpoint[46].x, Result3Dpoint[46].y, Result3Dpoint[46].z);
	point79->InsertPoint(4, Result3Dpoint[47].x, Result3Dpoint[47].y, Result3Dpoint[47].z);
	point79->InsertPoint(4, Result3Dpoint[48].x, Result3Dpoint[48].y, Result3Dpoint[48].z);


	vtkSmartPointer<vtkParametricSpline>spline79 = vtkSmartPointer<vtkParametricSpline>::New();
	spline79->SetPoints(point79);
	spline79->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource79 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource79->SetParametricFunction(spline79);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper79 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper79->SetInputConnection(splineSource79->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor79 = vtkSmartPointer<vtkActor>::New();
	splineActor79->SetMapper(splineMapper79);
	splineActor79->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube79 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube79->SetInput(splineSource79->GetOutput());
	tube79->SetInputConnection(splineSource79->GetOutputPort());
	tube79->SetNumberOfSides(24);
	tube79->SetRadius(2);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper79 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper79->SetInput(tube79->GetOutput());
	tubeMapper79->SetInputConnection(tube79->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor79 = vtkSmartPointer<vtkActor>::New();
	tubeActor79->SetMapper(tubeMapper79);
	tubeActor79->GetProperty()->SetColor(1, 0, 0);


	/*-----绘制第十段样条插值曲线以及圆柱体管道，从9点到10点-----*/
	vtkSmartPointer<vtkPoints>point910 = vtkSmartPointer<vtkPoints>::New();
	point910->InsertPoint(0, Result3Dpoint[48].x, Result3Dpoint[48].y, Result3Dpoint[48].z);
	point910->InsertPoint(1, Result3Dpoint[49].x, Result3Dpoint[49].y, Result3Dpoint[49].z);
	point910->InsertPoint(2, Result3Dpoint[50].x, Result3Dpoint[50].y, Result3Dpoint[50].z);
	point910->InsertPoint(3, Result3Dpoint[51].x, Result3Dpoint[51].y, Result3Dpoint[51].z);


	vtkSmartPointer<vtkParametricSpline>spline910 = vtkSmartPointer<vtkParametricSpline>::New();
	spline910->SetPoints(point910);
	spline910->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource910 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource910->SetParametricFunction(spline910);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper910 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper910->SetInputConnection(splineSource910->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor910 = vtkSmartPointer<vtkActor>::New();
	splineActor910->SetMapper(splineMapper910);
	splineActor910->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube910 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube910->SetInput(splineSource910->GetOutput());
	tube910->SetInputConnection(splineSource910->GetOutputPort());
	tube910->SetNumberOfSides(24);
	tube910->SetRadius(1.5);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper910 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper910->SetInput(tube910->GetOutput());
	tubeMapper910->SetInputConnection(tube910->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor910 = vtkSmartPointer<vtkActor>::New();
	tubeActor910->SetMapper(tubeMapper910);
	tubeActor910->GetProperty()->SetColor(1, 0, 0);



	/*-----绘制第十一段样条插值曲线以及圆柱体管道，从10点到11点-----*/
	vtkSmartPointer<vtkPoints>point1011 = vtkSmartPointer<vtkPoints>::New();
	point1011->InsertPoint(0, Result3Dpoint[48].x, Result3Dpoint[48].y, Result3Dpoint[48].z);
	point1011->InsertPoint(1, Result3Dpoint[52].x, Result3Dpoint[52].y, Result3Dpoint[52].z);
	point1011->InsertPoint(2, Result3Dpoint[53].x, Result3Dpoint[53].y, Result3Dpoint[53].z);
	point1011->InsertPoint(3, Result3Dpoint[54].x, Result3Dpoint[54].y, Result3Dpoint[54].z);
	point1011->InsertPoint(3, Result3Dpoint[55].x, Result3Dpoint[55].y, Result3Dpoint[55].z);

	vtkSmartPointer<vtkParametricSpline>spline1011 = vtkSmartPointer<vtkParametricSpline>::New();
	spline1011->SetPoints(point1011);
	spline1011->ClosedOff();

	vtkSmartPointer<vtkParametricFunctionSource>splineSource1011 = vtkSmartPointer<vtkParametricFunctionSource>::New();
	splineSource1011->SetParametricFunction(spline1011);

	vtkSmartPointer<vtkPolyDataMapper>splineMapper1011 = vtkSmartPointer<vtkPolyDataMapper>::New();
	splineMapper1011->SetInputConnection(splineSource1011->GetOutputPort());

	vtkSmartPointer<vtkActor>splineActor1011 = vtkSmartPointer<vtkActor>::New();
	splineActor1011->SetMapper(splineMapper1011);
	splineActor1011->GetProperty()->SetColor(0.38, 0.7, 0.16);

	vtkSmartPointer<vtkTubeFilter>tube1011 = vtkSmartPointer<vtkTubeFilter>::New();
	//tube1011->SetInput(splineSource1011->GetOutput());
	tube1011->SetInputConnection(splineSource1011->GetOutputPort());
	tube1011->SetNumberOfSides(24);
	tube1011->SetRadius(1.5);

	vtkSmartPointer<vtkPolyDataMapper>tubeMapper1011 = vtkSmartPointer<vtkPolyDataMapper>::New();
	//tubeMapper1011->SetInput(tube1011->GetOutput());
	tubeMapper1011->SetInputConnection(tube1011->GetOutputPort());

	vtkSmartPointer<vtkActor>tubeActor1011 = vtkSmartPointer<vtkActor>::New();
	tubeActor1011->SetMapper(tubeMapper1011);
	tubeActor1011->GetProperty()->SetColor(1, 0, 0);


	/*-----以下为vtk统一的绘制命令，向绘制窗口添加每一段样条曲线以及圆柱体管道的Actor-----*/
	vtkSmartPointer<vtkRenderer>ren1 = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow>renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer(ren1);
	vtkSmartPointer<vtkRenderWindowInteractor>iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);
	ren1->SetBackground(1, 1, 1);//设置绘制窗口背景

	//添加第1段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor01);
	ren1->AddActor(tubeActor01);
	//添加第2段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor12);
	ren1->AddActor(tubeActor12);
	//添加第3段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor23);
	ren1->AddActor(tubeActor23);
	//添加第4段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor24);
	ren1->AddActor(tubeActor24);
	//添加第5段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor45);
	ren1->AddActor(tubeActor45);
	//添加第6段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor46);
	ren1->AddActor(tubeActor46);
	//添加第7段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor17);
	ren1->AddActor(tubeActor17);
	//添加第8段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor78);
	ren1->AddActor(tubeActor78);
	//添加第9段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor79);
	ren1->AddActor(tubeActor79);
	//添加第10段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor910);
	ren1->AddActor(tubeActor910);
	//添加第11段样条曲线以及圆柱体管道的Actor
	ren1->AddActor(splineActor1011);
	ren1->AddActor(tubeActor1011);

	renWin->SetSize(500, 500);//设置窗口大小

	renWin->Render();//绘制
	iren->Start();//启动交互器

	return 0;
}