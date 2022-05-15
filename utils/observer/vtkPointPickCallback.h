#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkPointPicker.h"
#include "vtkImageActor.h"
#include "opencv2/opencv.hpp"

class vtkPointPickCallback : public vtkCommand
{
    public:
    static vtkPointPickCallback * New()
    {
        return new vtkPointPickCallback;
    }

    void SetSourceImages(cv::Mat &image1,cv::Mat &image2)
    {
        this->img1 = image1;
        this->img2 = image2;
    }


    void SetVTKActors(vtkImageActor* actor1, vtkImageActor* actor2)
    {
        this->actor1 = actor1;
        this->actor2 = actor2;
    }

    void SetRenderers(vtkRenderer* renderer1, vtkRenderer* renderer2)
    {
        this->renderer1 = renderer1;
        this->renderer2 = renderer2;
    }

    void SetInteractor(vtkRenderWindowInteractor* interactor)
    {
        this->interactor = interactor;
    }


    virtual void Execute(vtkObject* caller, unsigned long event, void* callData);


    private:
    cv::Mat img1;
    cv::Mat img2;
    vtkImageActor* actor1;
    vtkImageActor* actor2;
    vtkRenderer* renderer1;
    vtkRenderer* renderer2;
    vtkRenderWindowInteractor* interactor;
};

extern double* world_position;