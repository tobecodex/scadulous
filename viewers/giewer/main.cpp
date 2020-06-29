#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

#include "Vulkan.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanApp {

public:
  void run() 
  {
      initWindow();
      mainLoop();
      cleanup();
  }

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

    }

    void mainLoop() 
    {
      while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        _vulkan->drawFrame();
      }
    }

    void cleanup() 
    {
        if (_vulkan) {
          delete _vulkan;
          _vulkan = nullptr;
        }

        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void createInstance() {

    }
};

int main() {
    VulkanApp app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}