#ifndef VTK_VIEWER_H
#define VTK_VIEWER_H

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "QVTKWidget.h"

//体绘制之前需要进行相关初始化
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

/**
 * @brief 作为所有Viewer的基类，提供基础的参数和方法
 * 
 * @method void SetInputData(vtkImageData* data)
 * @method void SetRenderWindow(vtkRenderWindow* window)
 * @method void SetInteractorStyle(vtkInteractorObserver* style)
 * @method virtual void CreateActor();
 * @method void Start();
 * @method vtkRenderWindowInteractor* GetInteractor()
 * @method void AddObeserver(vtkCommand::EventIds event, vtkCommand* callback)
 * @method void AddActor(vtkProp* actor)
 * @method void Render()
 * 
 * @param data 由外部输入数据
 * @param main_actor 显示对象
 * @param renderer 由内部声明渲染器
 * @param window 由外部设置渲染窗口
 * @param interactor 由内部声明交互器 
 */
class vtkViewer
{
    public:
    vtkViewer()
    {
        renderer = vtkSmartPointer<vtkRenderer>::New();
    }
    ~vtkViewer(){};
    
    /**
     * @brief 设置输入数据
     * 
     * @param data 
     */
    void SetInputData(vtkImageData* data)
    {
        this->data = data;
    }

    /**
     * @brief 设置显示窗口
     * 
     * @param window 
     */
    void SetRenderWindow(vtkRenderWindow* window)
    {
        this->window = window;
    }

    /**
     * @brief 设置交互器
     * 
     * @param interactor 
     */
    void SetInteractor(vtkRenderWindowInteractor* interactor)
    {
        this->interactor = interactor;
    }

    /**
     * @brief 用于对interactor的指定信号添加响应
     * 
     * @param event 指定信号
     * @param callback 响应
     */
    void AddObserver(vtkCommand::EventIds event, vtkCommand* callback)
    {
        this->interactor->AddObserver(event, callback);
    }

    /**
     * @brief 设置交互风格
     * 
     * @param style 
     */
    void SetInteractorStyle(vtkInteractorObserver* style)
    {
        this->interactor->SetInteractorStyle(style);
    }

    /**
     * @brief 需要被重写，根据想要显示的主数据的不同来设置主演员main_actor，并将其添加到this->render中
     * 
     */
    virtual void CreateMainActor(){};

    /**
     * @brief 创建渲染管线
     * 
     */
    void Initial();

    /**
     * @brief 启动渲染管线
     * 
     */
    void Start();

    /**
     * @brief 返回在内部声明的window
     * 
     * @return vtkRenderWindow* 
     */
    vtkRenderWindow* GetRenderWindow()
    {
        return this->window;
    }

    /**
     * @brief 返回在内部声明的interactor
     * 
     * @return vtkRenderWindowInteractor* 
     */
    vtkRenderWindowInteractor* GetInteractor()
    {
        return this->interactor;
    }


    /**
     * @brief 添加除了主演员之外的actor
     * 
     * @param actor 
     */
    void AddActor(vtkProp* actor)
    {
        renderer->AddActor(actor);
    }

    /**
     * @brief 启动管线后添加新演员，需要重新渲染
     * 
     */
    void Render()
    {
        renderer->Render();
    }


    protected:
    vtkImageData* data;//由外部输入数据
    vtkSmartPointer<vtkRenderer> renderer;//由内部声明渲染器


    //交互显示
    vtkRenderWindow* window;//由外部设置渲染窗口
    vtkRenderWindowInteractor* interactor;//由外部设置交互器 
};

#endif