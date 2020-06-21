cmake_minimum_required(VERSION 3.0.0)
project(giewer VERSION 0.1.0)

include(CTest)
enable_testing()

set(VULKAN "C:\\VulkanSDK\\1.2.141.2")
set(GLFW "${CMAKE_SOURCE_DIR}/glfw-3.3.2")

set(GLFW_BUILD_DOCS OFF) 
set(GLFW_BUILD_TESTS OFF) 
set(GLFW_BUILD_EXAMPLES OFF) 
add_subdirectory(${GLFW})

add_executable(giewer main.cpp Vulkan.cpp)

target_include_directories(giewer PUBLIC "${GLFW}/include")
target_include_directories(giewer PUBLIC "${VULKAN}/Include")
target_include_directories(giewer PUBLIC "${CMAKE_SOURCE_DIR}")

target_link_libraries(giewer glfw ${GLFW_LIBRARIES})
target_link_libraries(giewer "${VULKAN}/Lib/Vulkan-1.lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
