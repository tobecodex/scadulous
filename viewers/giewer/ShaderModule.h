#ifndef __SHADER_MODULE_H
#define __SHADER_MODULE_H

#include <vector>
#include <vulkan/vulkan.h>

class ShaderModule
{
private:
  VkShaderModule _shaderModule;
  std::vector<char> loadShader(const char *path);

public:

  ShaderModule(const char *path);
  ~ShaderModule();

  operator VkShaderModule() const { return _shaderModule; }
};

#endif