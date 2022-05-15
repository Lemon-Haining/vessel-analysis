#include <shower.h>

int show_image(vtkImageData* image, vtkRenderWindow* window, vtkInteractorObserver* style)
{
    vtkSmartPointer<vtkImageActor> slice_actor = vtkSmartPointer<vtkImageActor>::New();
    slice_actor->SetInputData(image);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(slice_actor);
    renderer->SetBackground(1,1,1);
    
    window->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(window);
    interactor->SetInteractorStyle(style);

    interactor->Initialize();
    interactor->Start();
    std::cout << "slice show is over" << std::endl;
    return EXIT_SUCCESS;
}

int show_slices(vtkImageData* volume, 
                vtkRenderWindow* window, vtkInteractorObserver* style,
                double* normal, double* position)
{
    
    return EXIT_SUCCESS;
}