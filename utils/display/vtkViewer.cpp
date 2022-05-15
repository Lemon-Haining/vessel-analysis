#include "vtkViewer.h"

void vtkViewer::Start()
{    
    CreateMainActor();
    renderer->SetBackground(1,1,1);
    window->AddRenderer(renderer);
    interactor->SetRenderWindow(window);

    interactor->Initialize();
    interactor->Start();
}
