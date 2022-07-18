#include <iostream> 
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp> 
#include<opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <vector>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkTubeFilter.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkTriangleFilter.h>
#include <vtkCylinderSource.h>
#include <vtkTupleInterpolator.h>
#include <vtkDoubleArray.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>

using namespace cv; //使用OpenCV命名空间
using namespace std;//使用C++命名空间

#define  PI 3.14159265358979323846 //宏定义圆周率PI

typedef struct My_Scalefactor//定义缩放因子结构体
{
	double IamgeAXfactor;//ImageA上二维点X坐标除以D1
	double ImageAYfactor;//ImageA上二维点Y坐标除以D1
	double ImageBXfactor;//ImageB上二维点X坐标除以D2
	double ImageBYfactor;//ImageB上二维点X坐标除以D2
};
typedef struct My2dPoint//定义自己的二维坐标点结构体
{
	double x;
	double y;
};
typedef struct My3dPoint//定义自己的三维坐标点结构体
{
	double x;
	double y;
	double z;
};

void InitMat(Mat& m, double* num);  //用一维数组初始化OpenCV矩阵Mat对象
Mat GenerateRymat(double jiaodu);  //构造Ry矩阵OpenCV矩阵Mat对象,参数角度为角度，直接返回一个Mat对象
Mat GenerateRxmat(double jiaodu);  //构造Rx矩阵OpenCV矩阵Mat对象,参数角度为角度,直接返回一个Mat对象
float ExtractMatIJValue(Mat& M, int i, int j);  //提取Mat对象第i行第j列的数值,M为Mat对象，ij为行列值
Mat Generate3DAmat(Mat& M, My_Scalefactor& myfactor);  //生成最后求三维点的A矩阵，M为Mat类对象,为先求出的R矩阵，My_Scalefactor为struct
Mat Genreate3Damat(Mat& M, My_Scalefactor& myfactor);  //生成求B矩阵的a矩阵
Mat Generate3DTmat(double T);  //生成求B矩阵的T矩阵
Mat Generate3Dbmat(Mat& M, My_Scalefactor& myfactor);  //生成求B矩阵的b矩阵
Mat Generate3DBmat(Mat& a, Mat& b, Mat& t);  //生成最后求三维点的B矩阵
int vtkManual3DReconstruction();  
void vtkManual3DReconstructionRadious();