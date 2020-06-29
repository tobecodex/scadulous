#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

#include "Mesh.h"
#include "Vulkan.h"
#include "SocketServer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanApp
{
private:
  Vulkan *_vulkan;
  GLFWwindow *_window;

  static void createSurface(const VkInstance &vkInstance, VkSurfaceKHR *surface, void *userData)
  {
    VulkanApp *self = (VulkanApp *)(userData);
    if (glfwCreateWindowSurface(vkInstance, self->_window, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
  }

  void initWindow() 
  {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    uint32_t extCount = 0;
    const char **ext = glfwGetRequiredInstanceExtensions(&extCount);
    std::vector<const char *> extensions(ext, ext + extCount);

    _vulkan = new Vulkan(
      extensions,
      createSurface,
      this,
      std::vector<const char *>({ "VK_LAYER_KHRONOS_validation" })
    );

    std::vector<glm::vec3> tri = {
      { 0.0, -0.5, 0.0 },
      { 0.5,  0.5, 0.0 },
      { -0.5, 0.5, 0.0 }
    };

    Mesh mesh(tri);
    _vulkan->addMesh(mesh);
  }

  void mainLoop() 
  {
    while (!glfwWindowShouldClose(_window)) {
      glfwPollEvents();
      _vulkan->drawFrame();
    }
  }


public:

  ~VulkanApp() 
  {
    if (_vulkan) {
      delete _vulkan;
      _vulkan = nullptr;
    }

    glfwDestroyWindow(_window);
    glfwTerminate();
  }

  void run() 
  {
      initWindow();
      mainLoop();
  }

  void addMesh(const Mesh &mesh)
  {
    if (_vulkan) {
      _vulkan->addMesh(mesh);
    }
  }
};

class GiewerApp : public SocketClient
{
private:
  Mesh *_mesh;
  VulkanApp *_vulkanApp;

public:
  GiewerApp()
  {
    _vulkanApp = new VulkanApp();
  }

  ~GiewerApp()
  {
    if (_vulkanApp) {
      delete _vulkanApp;
      _vulkanApp = nullptr;
    }
  }

  void run()
  {
    if (_vulkanApp) {
      _vulkanApp->run();
    }
  }

  void onConnect(std::iostream &)
  {
  }

  void onClose(std::iostream &)
  {
  }
};

int main() 
{
  GiewerApp app;
  
  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}