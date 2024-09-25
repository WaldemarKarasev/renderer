#pragma once

// vulkan
#include <vulkan/vulkan.h>

namespace renderer::detail {

    bool IsDeviceSuitable(VkPhysicalDevice device);

} // namespace renderer::detail