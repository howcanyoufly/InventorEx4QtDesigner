cmake_minimum_required(VERSION 3.14)

project(InventorEx4QtDesigner)

set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "Configurations" FORCE)

# 设置Coin库目录
set(COIN3D_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Coin3dPack_x64_Debug" CACHE STRING "Path to Coin3D library")

# 找到QT组件
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL UiTools REQUIRED)

# 启用QT的自动处理工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 包含当前的二进制目录
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# 设置Coin的包含和库路径
include_directories(${COIN3D_PATH}/include)
link_directories(${COIN3D_PATH}/lib)

# Add the necessary compile definitions
add_definitions(-DQUARTER_DLL)

# Minimal example
add_executable(Example minimal.cpp)
target_link_libraries(Example PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::UiTools Coin4d.lib Quarter1d.lib)

# Dynamic UI Loader example
add_executable(DynamicUILoaderExample dynamicui.cpp quarterviewer.qrc)
target_link_libraries(DynamicUILoaderExample PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::UiTools Coin4d.lib Quarter1d.lib)

# Direct UI loader example
add_executable(DirectUILoaderExample directui.cpp QuarterViewer.ui)
target_link_libraries(DirectUILoaderExample PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::UiTools Coin4d.lib Quarter1d.lib)

# Examiner example
add_executable(ExaminerExample examiner.cpp quarterviewer.qrc)
target_link_libraries(ExaminerExample PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::UiTools Coin4d.lib Quarter1d.lib)

# Inherit UI example
add_executable(InheritUIExample inheritui.cpp
  QuarterViewer.cpp
  QuarterViewer.h
  QuarterViewer.ui
)
target_link_libraries(InheritUIExample PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::UiTools Coin4d.lib Quarter1d.lib)

# Multiple document interface example
add_executable(MDIExample mdi.cpp
  MdiMainWindow.cpp
  MdiMainWindow.h
  MdiQuarterWidget.cpp
  MdiQuarterWidget.h
)
target_link_libraries(MDIExample PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::UiTools Coin4d.lib Quarter1d.lib)

# Superimposition example
add_executable(SuperImpositionExample superimposition.cpp)
target_link_libraries(SuperImpositionExample PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::UiTools Coin4d.lib Quarter1d.lib)

# 定义一个包含所有目标的列表
set(ALL_TARGETS 
    Example 
    DynamicUILoaderExample 
    DirectUILoaderExample 
    ExaminerExample 
    InheritUIExample 
    MDIExample 
    SuperImpositionExample
)

# 为所有目标设置调试环境的PATH变量
foreach(TARGET_NAME ${ALL_TARGETS})
    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${COIN3D_PATH}/bin;C:/Qt/5.15.2/msvc2019_64/bin;%PATH%")
endforeach()
