cmake_minimum_required(VERSION 3.23)
project(LION)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#外1-引用外部库
#ITK
find_package(ITK REQUIRED)
set(ITK_INCLUDE_DIR "D:/software/path/C++Package/ITK/itk-5.1.2-install-MinGW-x64-Release/include/ITK-5.1")
#VTK
find_package(VTK REQUIRED)
set(VTK_INCLUDE_DIR "D:/software/path/C++Package/VTK/VTK-8.2.0-install-MinGW-x64-Release/include/vtk-8.2")
#Egien
set(Eigen3_DIR "D:/software/path/C++Package/Eigen/eigen-3.4.0-install-MinGW-x64-Release/share/eigen3/cmake")
find_package(Eigen3 REQUIRED)
#OpenCV
set(OpenCV_DIR "D:/software/path/C++Package/OpenCV/opencv-4.5.5-install-MinGW-x64-Release/x64/mingw/lib")
find_package(OpenCV REQUIRED)
#Qt
set(Qt5_DIR "D:/software/path/Qt/Qt5.14.2/5.14.2/mingw73_64/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Widgets Gui Core LinguistTools REQUIRED)

#外2-声明外部库
set(LION_EXTERNAL_LIBS
 ${ITK_LIBRARIES}
 ${VTK_LIBRARIES}
 ${EIGEN3_LIBRARIES}
 ${OpenCV_LIBS}
 Qt5::Widgets
)

#内1-声明内部库的源文件和头文件
#-UTILS
set(UTILS_SOURCE
 ${LION_SOURCE_DIR}/utils/IO/reader.cpp
 ${LION_SOURCE_DIR}/utils/IO/transfer.cpp
 ${LION_SOURCE_DIR}/utils/display/shower.cpp
 ${LION_SOURCE_DIR}/utils/display/vtkViewer.cpp
 ${LION_SOURCE_DIR}/utils/display/vtkSliceViewer.cpp
 ${LION_SOURCE_DIR}/utils/display/vtkVolumeViewer.cpp
 ${LION_SOURCE_DIR}/utils/observer/vtkPointPickCallback.cpp
)

set(UTILS_HEADER
 ${LION_SOURCE_DIR}/utils/IO/reader.h
 ${LION_SOURCE_DIR}/utils/IO/transfer.h
 ${LION_SOURCE_DIR}/utils/display/shower.h
 ${LION_SOURCE_DIR}/utils/display/vtkViewer.h
 ${LION_SOURCE_DIR}/utils/display/vtkSliceViewer.h
 ${LION_SOURCE_DIR}/utils/display/vtkVolumeViewer.h
 ${LION_SOURCE_DIR}/utils/observer/vtkPointPickCallback.h
)

#-GUI
set(GUI_SOURCE
 ${LION_SOURCE_DIR}/gui/dialog.cpp
 ${LION_SOURCE_DIR}/gui/mainwindow.cpp
)
set(GUI_HEADER
 ${LION_SOURCE_DIR}/gui/dialog.h
 ${LION_SOURCE_DIR}/gui/mainwindow.h
)
set(GUI_UI
 ${LION_SOURCE_DIR}/gui/dialog.ui
 ${LION_SOURCE_DIR}/gui/mainwindow.ui
)

#内2-创建内部库，并与外部库链接
add_library(LION_UTILS SHARED ${UTILS_SOURCE} ${UTILS_HEADER})
add_library(LION_GUI SHARED ${GUI_SOURCE} ${GUI_HEADER} ${GUI_UI})

target_link_libraries(LION_UTILS  ${LION_EXTERNAL_LIBS})
target_link_libraries(LION_GUI  ${LION_EXTERNAL_LIBS} ${LION_UTILS})

#内3-声明内部库
set(LION_INTERNAL_LIBS 
 LION_UTILS
 LION_GUI
)

#内4外3-声明头文件路径
set(LION_INCLUDE_DIRS
 ${LION_SOURCE_DIR}/utils/IO
 ${LION_SOURCE_DIR}/utils/display
 ${LION_SOURCE_DIR}/utils/observer
 ${LION_SOURCE_DIR}/gui
 ${ITK_INCLUDE_DIR}
 ${VTK_INCLUDE_DIR}
 ${EIGEN3_INCLUDE_DIR}
 ${OpenCV_INCLUDE_DIRS}
)

#为内部库设置头文件包含路径
foreach(target ${LION_INTERNAL_LIBS})
    target_include_directories(${target} PUBLIC ${LION_INCLUDE_DIRS})
endforeach()

#添加可执行文件
add_executable(LION main.cpp)
target_link_libraries(LION ${LION_INTERNAL_LIBS} ${LION_EXTERNAL_LIBS})
target_include_directories(LION PUBLIC ${LION_INCLUDE_DIRS})
