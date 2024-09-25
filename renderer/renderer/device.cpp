#include <renderer/renderer/device.hpp>

// std
#include <iostream>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// engine includes
#include <renderer/renderer/detail/debug_utils.hpp>
#include <renderer/renderer/detail/extension_utils.hpp>
#include <renderer/renderer/detail/validation_layers.hpp>
#include <renderer/renderer/detail/physical_device_utils.hpp>

namespace renderer
{

Device::Device(engine::Window& window)
    : window_(window)
{
    InitVulkan();
}

Device::~Device()
{
    detail::DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

void Device::InitVulkan()
{
    std::cout << "1" << std::endl;
    CreateInstance();
    std::cout << "2" << std::endl;
    SetupDebugMessenger();
    std::cout << "3" << std::endl;
    CreateSurface();
    std::cout << "4" << std::endl;
    PickPhysicalDevice();
    std::cout << "5" << std::endl;
}

void Device::CreateInstance()
{
    if (!detail::CheckValidationLayerSupport())
    {
        std::cerr << "Validation error not supported" << std::endl;
        std::abort();
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Vulkan Renderer";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "Home Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    auto extensions = detail::GetRequiredExtensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    std::cout << extensions.size() << std::endl;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    create_info.enabledLayerCount = static_cast<uint32_t>(detail::s_validation_layers_.size());
    create_info.ppEnabledExtensionNames = detail::s_validation_layers_.data();
    detail::PopulateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;

    std::cout << create_info.enabledLayerCount << std::endl;

    if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS)
    {
        std::cerr << "Failed to create instance!" << std::endl;
        std::abort();
    }
    std::cout << "6" << std::endl;
}

void Device::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    detail::PopulateDebugMessengerCreateInfo(create_info);

    if (detail::CreateDebugUtilsMessengerEXT(instance_, &create_info, nullptr, &debug_messenger_) != VK_SUCCESS)
    {
        std::cerr << "Failed to create CreateDebugUtilsMessengerEXT" << std::endl;
        std::abort();
    }
}

void Device::CreateSurface()
{
    window_.CreateSurface(instance_, &surface_);
}

void Device::PickPhysicalDevice()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

    if (device_count == 0)
    {
        std::cerr << "Failed to find any GPUs with Vulkan support!" << std::endl;
        std::abort();
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

    detail::PhysicalDeviceSelector s{devices, surface_};
    physical_device_ = s.Select();
    if (physical_device_ == VK_NULL_HANDLE)
    {
        std::cerr << "Failed to find a suitable GPU!" << std::endl;
        std::abort();
    }
}



} // namespace renderer
