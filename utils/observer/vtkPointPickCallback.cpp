#include "vtkPointPickCallback.h"
#include "transfer.h"

double* world_position;

void vtkPointPickCallback::Execute(vtkObject* caller, unsigned long event, void* callData)
{
    if(vtkCommand::LeftButtonPressEvent)
    {
        //获取屏幕坐标
        int event_position[2];
        interactor->GetEventPosition(event_position);
        std::cout << "even position:" << event_position[0] << "," << event_position[1]<< std::endl;


        int* window_size = this->interactor->GetRenderWindow()->GetSize();
        std::cout << "window size:" << window_size[0] << "," << window_size[1]<< std::endl;
        
        //通过光线投影法获取当前坐标
        if(event_position[0]<0.5*window_size[0])
        {
            interactor->GetPicker()->Pick(event_position[0],event_position[1],0,
                                        renderer1);
            world_position = interactor->GetPicker()->GetPickPosition();
        }
        else
        {
            interactor->GetPicker()->Pick(event_position[0],event_position[1],0,
                                        renderer2);
            world_position = interactor->GetPicker()->GetPickPosition();
        }

        std::cout << "world position:" << world_position[0] << "," << world_position[1]<< std::endl;

        //对图像的操作
        
        //对图像的操作
        
        vtkSmartPointer<vtkImageData> vtk_img1;
        vtk_img1=convertCVMatToVtkImageData(img2,true);
        actor1->SetInputData(vtk_img1);

        vtkSmartPointer<vtkImageData> vtk_img2;
        vtk_img2=convertCVMatToVtkImageData(img1,true);
        actor2->SetInputData(vtk_img2);
        renderer1->Render();
    }
}