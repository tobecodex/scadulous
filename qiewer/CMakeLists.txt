cmake_minimum_required(VERSION 3.10)

# your project name
project("qiewer")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5OpenGL)
find_package(OpenGL)
find_package(Threads)

set(sources
    src/app.cpp
    src/main.cpp
    src/mesh.cpp
    src/canvas.cpp
    src/window.cpp
    src/glmesh.cpp
    src/loader.cpp
)

add_executable(${CMAKE_PROJECT_NAME}  ${sources})

target_link_libraries(
  ${CMAKE_PROJECT_NAME}
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Gui
  ${CMAKE_THREAD_LIBS_INIT}
  ${OPENGL_LIBRARIES}
)
