#pragma once

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

namespace renderer::detail {

class PhysicalDeviceSelector
{
public:
    PhysicalDeviceSelector(std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR& surface)
        : devices_{devices}
        , surface_{surface}
    {}

    VkPhysicalDevice Select();

private:
    bool IsDeviceSuitable(VkPhysicalDevice device);

private:
    std::vector<VkPhysicalDevice>& devices_;
    VkSurfaceKHR& surface_;
};

} // namespace renderer::detail