# vessel-analysis

1.在main.cpp的path1和path2中设置两幅图片的路径

2.在/.utils/observer/vtkPointPickCallback.cpp中对应的位置写处理代码。

  -OpenCV读取的图像存放在类vtkPointPickCallback的成员变量img1和img2中，鼠标点击获取的点的坐标放在全局变量double* world_position中。
  
  -如果想将处理得到数据显示在第一个子窗口中，将显示交互部分中的convertCVMatToVtkImageData(img1,true)中的img1替换为想要显示的数据即可。
                     显示在第二个子窗口中，将显示交互部分中的convertCVMatToVtkImageData(img2,true)中的img2替换为想要显示的数据即可。
