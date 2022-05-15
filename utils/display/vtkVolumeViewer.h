#ifndef VTK_VOLUME_VIEWER_H
#define VTK_VOLUME_VIEWER_H

#include "vtkViewer.h"
#include "vtkVolume.h"

class vtkVolumeViewer : public vtkViewer
{
    public:
    vtkVolumeViewer(){
        main_actor = vtkSmartPointer<vtkVolume>::New();
    };
    ~vtkVolumeViewer(){};

    virtual void CreateMainActor() override;

    /**
     * @brief 计算输入数据的基本信息
     * space[3]: 三个方向上的像素间隔
     * extent[6]: 三个方向上的索引范围
     * origin[3]: 图像起点坐标
     * center[3]: 图像中心坐标
     */
    void InitDataInfo();

    private:
    
    //输入数据的信息
    double space[3];//像素间隔
    double origin[3];//图像索引起点[0,0,0]的物理世界坐标
    int extent[6];
    double center[3];//图像中心的物理世界坐标

    vtkSmartPointer<vtkVolume> main_actor;

};

#endif