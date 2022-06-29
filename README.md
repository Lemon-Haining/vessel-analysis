# vessel-analysis

1.在main.cpp的path1和path2中设置两幅图片的路径

2.在/.utils/observer/vtkPointPickCallback.cpp中对应的位置写处理代码。

  -OpenCV读取的图像存放在类vtkPointPickCallback的成员变量img1和img2中，鼠标点击获取的点的坐标放在全局变量double* world_position中。
  
  -如果想将处理得到数据显示在第一个子窗口中，将显示交互部分中的convertCVMatToVtkImageData(img1,true)中的img1替换为想要显示的数据即可。
                     显示在第二个子窗口中，将显示交互部分中的convertCVMatToVtkImageData(img2,true)中的img2替换为想要显示的数据即可。

6.23更新  
新增基于区域生长的RoI区域分割，能够有效分割两个点中间的部分，操作流程为：先点击选择血管的上下两个点，算法能够分割出上下两个点中间的区域  
对于区域生长的算法，其中如何聚类的条件还可以深入修改，当前使用的是手工设置像素阈值和选择的点处的像素值取平均值，后续还可以进行修改。
