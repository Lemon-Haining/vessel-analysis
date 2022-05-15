#ifndef TRANSFER_H
#define TRANSDER_H

#include <reader.h>
#include "itkImageToVTKImageFilter.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"
#include "opencv2/opencv.hpp"

//不同数据类型间的转换
int itk_to_vtk(ITKSliceType* itk_slice, vtkImageData* vtk_slice);
int itk_to_vtk(ITKVolumeType* itk_volume, vtkImageData* vtk_volume);
vtkSmartPointer<vtkImageData> convertCVMatToVtkImageData(const cv::Mat &sourceCVImage, bool flipOverXAxis);
//不同维度数据的转换
int volume_to_slice(vtkImageData* vtk_volume, vtkImageData* vtk_slice, double* normal, double* position);

#endif