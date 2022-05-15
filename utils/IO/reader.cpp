#include <reader.h>



/******** read volume *********/
int readDCMS(const char* dir, ITKVolumeType* volume)
{
    ITKVolumeReaderType::Pointer vol_reader = ITKVolumeReaderType::New();

    /*定义接口*/
    ITKDCMImageIOType::Pointer ImageIO = ITKDCMImageIOType::New();
    vol_reader->SetImageIO(ImageIO);
    
    /*生成文件名序列*/
    ITKGDCMNamesGenerator::Pointer names_generator = ITKGDCMNamesGenerator::New();
    names_generator->SetUseSeriesDetails(true);//告诉names_generator:使用.dcm文件的元信息（除去图像数据之外的信息）
    names_generator->AddSeriesRestriction("0008|0021");//使用"0008|0021"处的元信息（UID）
    names_generator->SetDirectory(dir);

    //
    std::vector<std::string>  file_ids = names_generator->GetSeriesUIDs();
    auto seriesItr = file_ids.begin();
    auto seriesEnd = file_ids.end();

    std::vector<std::string> file_names;
    std::string uid;
    while(seriesItr != seriesEnd)
    {
        uid = seriesItr->c_str();
        file_names = names_generator->GetFileNames(uid);//路径中含有多个dcm序列时，之输出最后一个序列
        ++seriesItr;
    }
    
    vol_reader->SetFileNames(file_names);

    try
    {
        vol_reader->Update();
    }
    catch(const itk::ExceptionObject& e)
    {
        std::cerr << "error caught in reading DCM Volume" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    volume->Graft(vol_reader->GetOutput());

    //设置图像起点和方向
    double origin[3] = {0,0,0};
    volume->SetOrigin(origin);
    using MatrixType = itk::Matrix<double, 3, 3>;
    MatrixType direction;
    direction.Fill(0.0f);
    direction(0,0) = 1;
    direction(1,1) = 1;
    direction(2,2) = 1;
    volume->SetDirection(direction);

    return 0;
}

int readNrrd(const char* path, ITKVolumeType* volume)
{
    ITKVolumeReaderType::Pointer reader = ITKVolumeReaderType::New();
    reader->SetFileName(path);

    ITKNrrdImageIOType::Pointer ImageIO = ITKNrrdImageIOType::New();
    reader->SetImageIO(ImageIO);


    try
    {
        reader->Update();
    }
    catch(const itk::ExceptionObject& err)
    {
        std::cerr << "error caught in reading .nrrd" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    volume->Graft(reader->GetOutput());

    return 0;
}

/******** read slice *********/
int readDCM(const char* path, ITKSliceType* image)
{
    ITKSliceReaderType::Pointer reader = ITKSliceReaderType::New();
    reader->SetFileName(path);

    ITKDCMImageIOType::Pointer dcmIO = ITKDCMImageIOType::New();
    reader->SetImageIO(dcmIO);

    try
    {
        reader->Update();
    }
    catch(const itk::ExceptionObject& err)
    {
        std::cerr << "error caught in reading DCM slice" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    image->Graft(reader->GetOutput());
    
    return 0;
}

void GetSize(ITKSliceType* image)
{
    ITKSliceType::RegionType region = image->GetLargestPossibleRegion();
    ITKSliceType::SizeType size = region.GetSize();

    std::cout<<size[0]<<","<<size[1]<<std::endl;
}

void GetSize(ITKVolumeType* volume)
{
    ITKVolumeType::RegionType region = volume->GetLargestPossibleRegion();
    ITKVolumeType::SizeType size = region.GetSize();

    std::cout<<size[0]<<","<<size[1]<<","<<size[2]<<std::endl;
}