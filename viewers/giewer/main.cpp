#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"

#include <map>
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
  static std::map<GLFWwindow *, VulkanApp *> _windowToApp;

  void initWindow() 
  {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    _windowToApp[_window] = this;

    uint32_t extCount = 0;
    const char **ext = glfwGetRequiredInstanceExtensions(&extCount);
    std::vector<const char *> extensions(ext, ext + extCount);

    VkSurfaceKHR surface;
    _vulkan = new Vulkan(extensions, std::vector<const char *>({ "VK_LAYER_KHRONOS_validation" }));
    if (glfwCreateWindowSurface(_vulkan->instance(), _window, nullptr, &surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
    _vulkan->setSurface(surface);

    glfwSetKeyCallback(_window, handleKeyboardInput);
    createGeometry();   
  }

  void createGeometry()
  {
    std::vector<glm::vec3> tri = {
      { 0.0, 0.5, 0.0 },
      { 0.5, -0.5, 0.0 },
      { -0.5, -0.5, 0.0 }
    };

    _vulkan->addMesh(new Mesh(tri));
  }

  void update()
  {
    auto meshes = _vulkan->state().meshes();
    for (auto m : meshes) {
      m->transform(glm::rotate(m->transform(), 0.0001f, glm::vec3(0, 1, 0)));
    }
  }

  void mainLoop() 
  {
    while (!glfwWindowShouldClose(_window)) {
      glfwPollEvents();
      update();
    }
  }

  /*
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  _cameraMatrix.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  _cameraMatrix.view = glm::lookAt({1, 1, 1}, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  _cameraMatrix.proj = glm::perspective(glm::radians(45.0f), _swapChain->extent().width / (float)_swapChain->extent().height, 0.1f, 10.0f);
  _cameraMatrix.proj[1][1] *= -1;
  */

  void onKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
      switch (key)
      {
        case GLFW_KEY_W : {
          _vulkan->state().camera().forward(0.1f);
        }
        break;

        case GLFW_KEY_S : {
          _vulkan->state().camera().backward(0.1f);
        }
        break;

        case GLFW_KEY_A : {
          _vulkan->state().camera().left(0.1f);
        }
        break;
   
        case GLFW_KEY_D : {
          _vulkan->state().camera().right(0.1f);
        }
        break;

        case GLFW_KEY_2 : {
          _vulkan->state().camera().up(0.1f);
        }
        break;

        case GLFW_KEY_X : {
          _vulkan->state().camera().down(0.1f);
        }
        break;

        case GLFW_KEY_Q : {
          _vulkan->state().camera().yaw(-0.1f);
        }
        break;

        case GLFW_KEY_E : {
          _vulkan->state().camera().yaw(0.1f);
        }
        break;

        case GLFW_KEY_R : {
          _vulkan->state().camera().pitch(-0.1f);
        }
        break;

        case GLFW_KEY_F : {
          _vulkan->state().camera().pitch(0.1f);
        }
        break;

        case GLFW_KEY_Z : {
          _vulkan->state().camera().roll(-0.1f);
        }
        break;

        case GLFW_KEY_C : {
          _vulkan->state().camera().roll(0.1f);
        }
        break;

        case GLFW_KEY_O : {
        }
        break;

        case GLFW_KEY_SPACE : {
          _vulkan->state().camera().lookAt(0,0,0);
        }
        break;
      }
    }
  }

  static void handleKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    _windowToApp[window]->onKeyboardInput(window, key, scancode, action, mods);
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
      _vulkan->addMesh(m);
    }
  }

  void onClose(std::iostream &)
  {
  }
};

std::map<GLFWwindow *, VulkanApp *> VulkanApp::_windowToApp;

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