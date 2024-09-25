#pragma once

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// engine includes
#include <renderer/window.hpp>

namespace renderer {

class Device
{
public:
    Device(engine::Window& window);
    ~Device();
private:
    void InitVulkan();

    // Instance
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();

private:
    // window for rendering to it
    engine::Window& window_;

    VkInstance instance_;
    VkDebugUtilsMessengerEXT debug_messenger_;

    // surface
    VkSurfaceKHR surface_;

    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;



private:

};

} // namespace renderer