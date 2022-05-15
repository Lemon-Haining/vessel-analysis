#include "vtkSliceViewer.h"
#include "transfer.h"
#include "math.h"


void vtkSliceViewer::CreateMainActor()
{
    main_actor = vtkSmartPointer<vtkImageActor>::New();
    main_actor->SetInputData(data);
    renderer->AddActor(main_actor);
}

//设置切片坐标系：格式为4x4的矩阵，该矩阵为射影矩阵，前3x3矩阵的列向量分别代表切片坐标系x，y，z三个轴的方向；第四列代表切片坐标系的原点在世界坐标系中的位置
//其中x轴，y轴为切平面，z轴垂直于切平面。
//例如：
//{0,1,0,0, 
// 1,0,0,0,
// 0,0,1,0,
// 0,0,0,1
//}
//代表x轴方向为[0,1,0],y轴方向为[1,0,0],z轴方向为[0,0,1],切片坐标系原点为[0,0,0]
void vtkSliceViewer::SetBasis(double* basis)
{
    this->basis = basis;
}

//设置切片坐标系z轴方向上的间隔
void vtkSliceViewer::SetNormalSpace(double normal_space)
{
    this->normal_space = normal_space;
}

void vtkSliceViewer::InitDataInfo()
{

    //获取数据的间隔、原点坐标、索引值范围
    data->GetSpacing(this->space);
    data->GetOrigin(this->origin);
    data->GetExtent(this->extent);

    //获取数据中心点坐标
    this->center[0] = origin[0] + space[0]*0.5*(extent[1]-extent[0]);
    this->center[1] = origin[1] + space[1]*0.5*(extent[3]-extent[2]);
    this->center[2] = origin[2] + space[2]*0.5*(extent[5]-extent[4]);

    //计算世界物理坐标系下，输入的三维数据的对角线向量
    double diag[3];
    diag[0] = space[0]*(extent[1]-extent[0]+1);
    diag[1] = space[1]*(extent[3]-extent[2]+1);
    diag[2] = space[2]*(extent[5]-extent[4]+1);

    //计算切片坐标系z轴的长度 = <diag,z>（内积）
    int normal_length;
    normal_length = abs(diag[0]*basis[2] + diag[1]*basis[6] + diag[2]*basis[10]);

    //获取切片序号的最大值和最小值
    this->max = int(normal_length / normal_space/2);
    this->min = -int(normal_length / normal_space/2);
}

/*数据提取接口*/
void vtkSliceViewer::SetSlice(int slice_number)
{
    if(slice_number > max || slice_number < min)
    {
        std::cout << "error: the slice is out of volume" << std::endl;
        return;
    }

    position[0] = center[0] + slice_number * normal_space * basis[2];
    position[1] = center[1] + slice_number * normal_space * basis[6];
    position[2] = center[2] + slice_number * normal_space * basis[10];

    volume_to_slice(data, slice, basis, position);
}


void vtkSliceViewer::SetPointPicker()
{
    interactor->SetPicker(point_picker);
}



int vtkSliceViewer::GetSliceMax()
{
    return max;
}
int vtkSliceViewer::GetSliceMin()
{
    return min;
}

