#include "vtkVolumeViewer.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkImageCast.h"

void vtkVolumeViewer::CreateMainActor()
{
    //创建体数据演员
    vtkNew<vtkSmartVolumeMapper> mapper;
    mapper->SetInputData(data);
    mapper->SetBlendModeToComposite();

    vtkNew<vtkVolumeProperty> property;
    property->ShadeOff();
    property->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

    vtkNew<vtkPiecewiseFunction> opacityFun;
    opacityFun->AddPoint(-3024, 0, 0.5, 0.0);
    opacityFun->AddPoint(-1000, 0, 0.5, 0.0);
    opacityFun->AddPoint(-500, 1.0, 0.33, 0.45);
    opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);
    property->SetScalarOpacity(opacityFun); // composite first.

    vtkNew<vtkColorTransferFunction> colorFun;
    colorFun->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
    colorFun->AddRGBPoint(-1000, .62, .36, .18, 0.5, 0.0);
    colorFun->AddRGBPoint(-500, .88, .60, .29, 0.33, 0.45);
    colorFun->AddRGBPoint(3071, .83, .66, 1, 0.5, 0.0);
    property->SetColor(colorFun);

    main_actor->SetMapper(mapper);
    main_actor->SetProperty(property);

    //添加体数据演员
    renderer->AddVolume(main_actor);
}

void vtkVolumeViewer::InitDataInfo()
{
    //获取数据的间隔、原点坐标、索引值范围
    data->GetSpacing(this->space);
    data->GetOrigin(this->origin);
    data->GetExtent(this->extent);

    //获取数据中心点坐标
    this->center[0] = origin[0] + space[0]*0.5*(extent[1]-extent[0]);
    this->center[1] = origin[1] + space[1]*0.5*(extent[3]-extent[2]);
    this->center[2] = origin[2] + space[2]*0.5*(extent[5]-extent[4]);
}