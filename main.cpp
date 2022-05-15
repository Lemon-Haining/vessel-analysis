//自定义库
#include "reader.h"
#include "transfer.h"
#include "shower.h"
#include "vtkSliceViewer.h"
#include "vtkVolumeViewer.h"
#include "dialog.h"
#include "mainwindow.h"

//系统库
#include <iostream>

//VTK库
#include "vtkPointPickCallback.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "itkImageRegionConstIterator.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkAxesActor.h"
#include "vtkJPEGReader.h"

//OpenCV库
#include "opencv2/opencv.hpp"

//Qt库
#include <QApplication>
#include <QDialog>


using namespace cv;


void GetImageInfo(vtkImageData* img)
{
    std::cout << std::endl;
    std::cout << "/************VTK Image Info***********/" << std::endl;
    std::cout << "origin:" << img->GetOrigin()[0] << "," << img->GetOrigin()[1] << "," << img->GetOrigin()[2] << std::endl;
    std::cout << "space:" << img->GetSpacing()[0] << "," << img->GetSpacing()[1] << "," << img->GetSpacing()[2] << std::endl;
    std::cout << "extent start:" << img->GetExtent()[0] << "," << img->GetExtent()[2] << "," << img->GetExtent()[4] << std::endl;
    std::cout << "extent end:" << img->GetExtent()[1] << "," << img->GetExtent()[3] << "," << img->GetExtent()[5] << std::endl;
    std::cout << std::endl;
}

int main(int argc,char** argv)
{

    char path1[] = "C:/Users/LION/Desktop/1.jpg";
    char path2[] = "C:/Users/LION/Desktop/2.jpg";

    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetPosition(0,0,0);
    axes->SetTotalLength(300,300,300);
    axes->SetAxisLabels(0);

    // vtkSmartPointer<vtkJPEGReader> reader1 = vtkSmartPointer<vtkJPEGReader>::New();
    // reader1->SetFileName(path1);
    // reader1->Update();

    //第一幅图片
    Mat img1= imread(path1);
    vtkSmartPointer<vtkImageData> vtk_img1;
    vtk_img1=convertCVMatToVtkImageData(img1,true);
    GetImageInfo(vtk_img1);

    vtkSmartPointer<vtkImageActor> actor1 = vtkSmartPointer<vtkImageActor>::New();
    actor1->SetInputData(vtk_img1);
    vtkSmartPointer<vtkRenderer> renderer1 = vtkSmartPointer<vtkRenderer>::New();
    renderer1->AddActor(actor1);
    renderer1->AddActor(axes);
    renderer1->SetViewport(0.0,0.0,0.5,1.0);

    //第二幅图片
    Mat img2 = imread(path2);
    vtkSmartPointer<vtkImageData> vtk_img2;
    vtk_img2=convertCVMatToVtkImageData(img2,true);
    GetImageInfo(vtk_img2);

    vtkSmartPointer<vtkImageActor> actor2 = vtkSmartPointer<vtkImageActor>::New();
    actor2->SetInputData(vtk_img2);
    vtkSmartPointer<vtkRenderer> renderer2 = vtkSmartPointer<vtkRenderer>::New();
    renderer2->AddActor(actor2);
    renderer2->AddActor(axes);
    renderer2->SetViewport(0.5,0.0,1.0,1.0);

    //显示及交互
    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer1);
    window->AddRenderer(renderer2);

    vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(window);
    interactor->SetInteractorStyle(style);

    vtkSmartPointer<vtkPointPickCallback> observer = vtkSmartPointer<vtkPointPickCallback>::New();
    observer->SetSourceImages(img1,img2);
    observer->SetVTKActors(actor1,actor2);
    observer->SetRenderers(renderer1,renderer2);
    observer->SetInteractor(interactor);


    interactor->AddObserver(vtkCommand::LeftButtonPressEvent, observer);



    interactor->Initialize();
    interactor->Start();


    return 0;
}


