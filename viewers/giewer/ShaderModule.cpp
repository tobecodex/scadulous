#include "ShaderModule.h"

#include <fstream>

#include "Vulkan.h"

ShaderModule::ShaderModule(const char *path)
{
  std::vector<char> shader = loadShader(path);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shader.size();
  createInfo.pCode = (const uint32_t*)(shader.data());

  if (vkCreateShaderModule(Vulkan::ctx().device(), &createInfo, nullptr, &_shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }
}

ShaderModule::~ShaderModule()
{
  vkDestroyShaderModule(Vulkan::ctx().device(), _shaderModule, nullptr);
}

std::vector<char> ShaderModule::loadShader(const char *path)
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