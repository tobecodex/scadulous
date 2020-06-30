#ifndef __COMMAND_QUEUES_H
#define __COMMAND_QUEUES_H

#include <optional>
#include "vulkan/vulkan.h"
#include "PhysicalDevice.h"

class CommandQueues
{
public:
  std::optional<uint32_t> _graphicsFamily;
  std::optional<uint32_t> _presentationFamily;
  
  CommandQueues(PhysicalDevice &device, VkSurfaceKHR &surface);
  ~CommandQueues();
};

#endif