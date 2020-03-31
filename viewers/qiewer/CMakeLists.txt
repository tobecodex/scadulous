cmake_minimum_required(VERSION 3.10)

# your project name
project("qiewer")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_PREFIX_PATH "C:\\Qt\\Qt-5.14.2")

find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Threads)

set(sources
  src/App.cpp
  src/main.cpp
  src/Mesh.cpp
  src/GLMesh.cpp
  src/Loader.cpp
  src/OpenGLWindow.cpp
)

add_executable(${CMAKE_PROJECT_NAME}  ${sources})

target_link_libraries(
  ${CMAKE_PROJECT_NAME}
  Qt5::Widgets
  Qt5::Gui
  ${CMAKE_THREAD_LIBS_INIT}
  ${OPENGL_LIBRARIES}
)
