#include "CommandQueues.h"

#include <algorithm>

CommandQueues::CommandQueues(PhysicalDevice &device, VkSurfaceKHR &surface)
{
  std::vector<uint32_t> intersection;
  std::vector<uint32_t> graphicsFamilies = device.getQueueFamilies(VK_QUEUE_GRAPHICS_BIT);
  std::vector<uint32_t> presentationFamilies = device.getPresentationQueues(surface);

  std::sort(graphicsFamilies.begin(), graphicsFamilies.end()); 
  std::sort(presentationFamilies.begin(), presentationFamilies.end());

  std::set_intersection(
    graphicsFamilies.begin(), graphicsFamilies.end(),
    presentationFamilies.begin(), presentationFamilies.end(),
    std::back_inserter(intersection)
  );

  if (!intersection.empty()) {
    _graphicsFamily = _presentationFamily = intersection[0];
  }
  else {
    _graphicsFamily = graphicsFamilies[0];
    _presentationFamily = presentationFamilies[0];
  }
}

CommandQueues::~CommandQueues()
{
}