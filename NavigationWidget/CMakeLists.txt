cmake_minimum_required(VERSION 3.10)

# 项目名称
project(NavigationWidget)

# 设置使用的 Qt5 路径和 Coin3D 路径
set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019_64")
set(COIN3D_INCLUDE_DIR "C:/Users/Admin/Documents/WorkingSpace/coin3d/include")
set(COIN3D_LIBRARY_DIR "C:/Users/Admin/Documents/WorkingSpace/coin3d/lib")

# 寻找 Qt5 相关组件
find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL REQUIRED)

# 包含 Qt5 和 Coin3D 的头文件目录
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIR})

# 指定库文件的搜索路径
link_directories(${COIN3D_LIBRARY_DIR})

# 添加定义，启用 MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 指定源文件
set(SOURCE_FILES
    CommonHeaderFiles.cpp
    NavigationStyle.cpp
    NavigationWidget.cpp
)

# 指定头文件
set(HEADER_FILES
    CommonHeaderFiles.h
    NavigationStyle.h
    NavigationWidget.h
)

# 创建静态链接库
add_library(NavigationWidget STATIC ${SOURCE_FILES} ${HEADER_FILES})

# 链接 Qt5 和 Coin3D 库
target_link_libraries(NavigationWidget Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL Coin4d.lib Quarter1d.lib)

# 指定安装路径
install(TARGETS NavigationWidget DESTINATION "C:/Users/Admin/Documents/WorkingSpace/InventorEx4QtDesigner/NavigationWidget")
