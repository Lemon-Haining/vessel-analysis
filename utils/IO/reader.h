#ifndef LION_READER_H
#define LION_READER_H


#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkNrrdImageIO.h"
#include "itkMatrix.h"

//image type
using ITKVolumeType = itk::Image<unsigned char, 3>;
using ITKSliceType = itk::Image<unsigned char, 2>;

//reader type
using ITKVolumeReaderType = itk::ImageSeriesReader<ITKVolumeType>;
using ITKSliceReaderType = itk::ImageFileReader<ITKSliceType>;

//names generator
using ITKGDCMNamesGenerator = itk::GDCMSeriesFileNames;

//IO type
using ITKDCMImageIOType = itk::GDCMImageIO;
using ITKNrrdImageIOType = itk::NrrdImageIO;


//read volume
int readDCMS(const char* dir, ITKVolumeType* volume);
int readNrrd(const char* path, ITKVolumeType* volume);

//read slice
int readDCM(const char* path, ITKSliceType* image);


void GetSize(ITKSliceType* image);
void GetSize(ITKVolumeType* volume);
#endif