cmake_minimum_required(VERSION 3.16)
project(Drawer)

set(CMAKE_CXX_STANDARD 17)

# 屏蔽 macOS OpenGL 弃用警告
add_definitions(-DGL_SILENCE_DEPRECATION)

# OpenGL 系统库
find_package(OpenGL REQUIRED)

# GLFW Homebrew 路径
set(GLFW_INCLUDE_DIR "/opt/homebrew/opt/glfw/include")
set(GLFW_LIBRARY "/opt/homebrew/opt/glfw/lib/libglfw.dylib")

# 添加可执行文件
add_executable(Drawer src/main.cpp)

# 指定头文件搜索路径
target_include_directories(Drawer PRIVATE ${GLFW_INCLUDE_DIR})

# 链接库（绝对路径确保找到）
target_link_libraries(Drawer ${GLFW_LIBRARY} ${OPENGL_LIBRARIES})