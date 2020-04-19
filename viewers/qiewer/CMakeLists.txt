cmake_minimum_required(VERSION 3.10)

# your project name
project("qiewer")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt-5.14.2")

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)

find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Boost 1.72)

set(sources
  src/App.cpp
  src/main.cpp
  src/Mesh.cpp
  src/Loader.cpp
  src/Camera.cpp
  src/MainWindow.cpp
  src/OpenGLWindow.cpp
  src/SocketServer.cpp
)

add_definitions(-DWIN32_LEAN_AND_MEAN -DBOOST_ASIO_NO_DEPRECATED)

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_executable(${CMAKE_PROJECT_NAME}  ${sources})

target_link_libraries(
  ${CMAKE_PROJECT_NAME}
  Qt5::Widgets
  Qt5::Gui
  ${CMAKE_THREAD_LIBS_INIT}
  ${OPENGL_LIBRARIES}
  ${Boost_LIBRARIES}
)
