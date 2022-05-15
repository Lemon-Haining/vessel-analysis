#include <transfer.h>
#include "vtkInformation.h"

//不同数据类型间的转换
int itk_to_vtk(ITKSliceType* itk_slice, vtkImageData* vtk_slice)
{
    using TransferType = itk::ImageToVTKImageFilter<ITKSliceType>;
    TransferType::Pointer transfer = TransferType::New();
    transfer->SetInput(itk_slice);

    try
    {
        transfer->Update();
    }
    catch(const itk::ExceptionObject& e)
    {
        std::cerr << "error caught in transfering ITK slice to VTK slice" << std::endl;
        std::cerr << e.what() << '\n';
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    vtk_slice->DeepCopy(transfer->GetOutput());
    return 0;
}

int itk_to_vtk(ITKVolumeType* itk_volume, vtkImageData* vtk_volume)
{
    using TransferType = itk::ImageToVTKImageFilter<ITKVolumeType>;
    TransferType::Pointer transfer = TransferType::New();
    transfer->SetInput(itk_volume);

    try
    {
        transfer->Update();
    }
    catch(const itk::ExceptionObject& e)
    {
        std::cerr << "error caught in transfering ITK volume to VTK volume" << std::endl;
        std::cerr << e.what() << '\n';
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    vtk_volume->DeepCopy(transfer->GetOutput());
    return 0;
}


//不同维度间的转换
int volume_to_slice(vtkImageData* vtk_volume, vtkImageData* vtk_slice, double* normal, double* position)
{

    vtkSmartPointer<vtkMatrix4x4> plane = vtkSmartPointer<vtkMatrix4x4>::New();
    plane->DeepCopy(normal);
    plane->SetElement(0,3,position[0]);
    plane->SetElement(1,3,position[1]);
    plane->SetElement(2,3,position[2]);

    vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInputData(vtk_volume);
    reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxes(plane);
    reslice->SetInterpolationModeToLinear();

    try
    {
        reslice->Update();
    }
    catch(const itk::ExceptionObject& e)
    {
        std::cerr << "error caught in transfering volume to slice" << std::endl;
        std::cerr << e.what() << '\n';
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    vtk_slice->DeepCopy(reslice->GetOutput());
    return 0;
}


vtkSmartPointer<vtkImageData> convertCVMatToVtkImageData(const cv::Mat &sourceCVImage, bool flipOverXAxis) {
  vtkSmartPointer<vtkImageData> outputVtkImage = vtkSmartPointer<vtkImageData>::New();

  int numOfChannels = sourceCVImage.channels();

  // dimension set to 1 for z since it's 2D
  outputVtkImage->SetDimensions(sourceCVImage.cols, sourceCVImage.rows, 1);

  // NOTE: if your image isn't uchar for some reason you'll need to change this type
  outputVtkImage->AllocateScalars(VTK_UNSIGNED_CHAR, numOfChannels);

  // the flipped image data gets put into tempCVImage
  cv::Mat tempCVImage;
  if(flipOverXAxis){ // Normally you should flip the image!
    cv::flip(sourceCVImage, tempCVImage, 0);
  }
  else {
    tempCVImage = sourceCVImage;
  }

  // the number of byes in the cv::Mat, assuming the data type is uchar
  size_t byte_count = sourceCVImage.cols * sourceCVImage.rows * numOfChannels * sizeof(unsigned char);

  // copy the internal cv::Mat data into the vtkImageData pointer
  memcpy(outputVtkImage->GetScalarPointer(), tempCVImage.data, byte_count);

  outputVtkImage->Modified();
  return outputVtkImage;
}
