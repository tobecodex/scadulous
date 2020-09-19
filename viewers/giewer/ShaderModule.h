#ifndef __SHADER_MODULE_H
#define __SHADER_MODULE_H

#include <vulkan/vulkan.h>
#include <string>
#include <fstream>

class ShaderModule
{
private:
  const VkDevice &_device;
  VkShaderModule _shaderModule;

  std::vector<char> loadShader(const std::string &path)
  {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
  }

public:

  ShaderModule(const VkDevice &device, const std::string &path) : _device(device)
  {
    std::vector<char> shader = loadShader(path);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shader.size();
    createInfo.pCode = (const uint32_t*)(shader.data());

    if (vkCreateShaderModule(device, &createInfo, nullptr, &_shaderModule) != VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
    }
  }

  ~ShaderModule()
  {
    vkDestroyShaderModule(_device, _shaderModule, nullptr);
  }

  operator VkShaderModule() const { return _shaderModule; }
};

#endif