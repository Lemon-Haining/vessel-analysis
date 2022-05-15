#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "vtkVolumeViewer.h"
#include "reader.h"
#include "transfer.h"
#include "shower.h"
#include "vtkSliceViewer.h"
#include "vtkVolumeViewer.h"
#include "dialog.h"
#include "mainwindow.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkAxesActor.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}