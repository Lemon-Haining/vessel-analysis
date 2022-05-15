#ifndef SHOWER_H
#define SHOWER_H

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


//函数名：show_slice
//作者：赵海宁
//时间：2022.04.05
//参数：slice-二维图像数据；window-渲染窗口；style-交互风格
//返回值：0-正常退出标识符
//作用：在渲染窗口window处显示图像slice，并采用style作为交互风格
int show_image(vtkImageData* image, 
                vtkRenderWindow* window, vtkInteractorObserver* style);

//函数名：show_slices
//作者：赵海宁
//时间：2022.04.05
//参数：slice-二维图像数据；window-渲染窗口；style-交互风格；normal-法向量
//返回值：0-正常退出标识符
//作用：在渲染窗口window处显示图像slice，并采用style作为交互风格
int show_slices(vtkImageData* volume, vtkRenderWindow* window, 
                vtkInteractorObserver* style, vtkMatrix4x4* normal);

#endif