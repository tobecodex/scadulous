#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include <iostream>
#include <cstdlib>

#include "Mesh.h"
#include "Vulkan.h"
#include "Loader.h"
#include "SocketServer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanApp : public SocketClient
{
private:
  Vulkan *_vulkan;
  GLFWwindow *_window;
  SocketServer _socketServer;

  void initWindow() 
  {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    uint32_t extCount = 0;
    const char **ext = glfwGetRequiredInstanceExtensions(&extCount);
    std::vector<const char *> extensions(ext, ext + extCount);

    VkSurfaceKHR surface;
    _vulkan = new Vulkan(extensions, std::vector<const char *>({ "VK_LAYER_KHRONOS_validation" }));
    if (glfwCreateWindowSurface(*_vulkan, _window, nullptr, &surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
    _vulkan->setSurface(surface);

    createGeometry();   
  }

  void createGeometry()
  {
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
      _vulkan->draw();
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
      _socketServer.start(*this);
      mainLoop();
  }

  void onConnect(std::iostream &s)
  {
    Loader loader;
    std::string str(std::istreambuf_iterator<char>(s), {});
    std::istringstream inp(str);
    Mesh *m = loader.load_stl(inp);
    if (_vulkan) {
      _vulkan->addMesh(*m);
    }
    delete m;
  }

  void onClose(std::iostream &)
  {
  }
};

int main() 
{
  VulkanApp app;
  
  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}