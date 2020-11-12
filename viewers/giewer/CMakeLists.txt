cmake_minimum_required(VERSION 3.0.0)
project(giewer VERSION 0.1.0)

include(CTest)
enable_testing()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.72 REQUIRED)

set(GLM ${CMAKE_SOURCE_DIR}/glm)
set(GLFW ${CMAKE_SOURCE_DIR}/glfw)
set(VULKAN "C:\\VulkanSDK\\1.2.141.2")
set(GLSLC "${VULKAN}\\Bin\\glslc.exe")
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/shaders")
file(GLOB SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag)
message(${SHADERS})

set(SHADER_SPV, [])
foreach(shader ${SHADERS})
  list(APPEND SHADER_SPV ${shader}.spv)
  add_custom_command(OUTPUT ${shader}.spv COMMAND ${GLSLC} ${shader} -o ${shader}.spv DEPENDS ${shader})
endforeach()

set(GLFW_BUILD_DOCS OFF) 
set(GLFW_BUILD_TESTS OFF) 
set(GLFW_BUILD_EXAMPLES OFF) 
add_subdirectory(${GLFW})

add_definitions(-DWIN32_LEAN_AND_MEAN -D_WIN32_WINDOWS -DBOOST_ASIO_NO_DEPRECATED)

set(sources
  main.cpp
  Mesh.cpp
  Vulkan.cpp
  Loader.cpp
  Camera.cpp
  SwapChain.cpp
  SocketServer.cpp
  Device.cpp
  GraphicsPipeline.cpp
  CommandPool.cpp
  CommandBuffer.cpp
  DescriptorPool.cpp
  ResourceBuffer.cpp
  DescriptorSet.cpp
  DescriptorSetLayout.cpp
  PhysicalDevice.cpp
  Logger.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${sources} ${SHADER_SPV})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLM})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${GLFW}/include)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${VULKAN}/Include)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${CMAKE_PROJECT_NAME} ${VULKAN}/Lib/Vulkan-1.lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
