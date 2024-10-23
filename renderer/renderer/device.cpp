#include <renderer/renderer/device.hpp>

// std
#include <iostream>
#include <set>

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
    vkDestroyCommandPool(device_, command_pool_, nullptr);
    vkDestroyDevice(device_, nullptr);

    // Validation layers

    detail::DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);

    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

void Device::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateCommapdPool();
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
    app_info.pEngineName = "No Engine";
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
    // create_info.ppEnabledExtensionNames = detail::s_validation_layers_.data();
    create_info.ppEnabledLayerNames = detail::s_validation_layers_.data();

    detail::PopulateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;


    if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS)
    {
        std::cerr << "Failed to create instance!" << std::endl;
        std::abort();
    }
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

    physical_device_ = detail::PhysicalDeviceSelector{devices, surface_}.Select(); // CE: because of & in Select function declaration

    if (physical_device_ == VK_NULL_HANDLE)
    {
        std::cerr << "Failed to find a suitable GPU!" << std::endl;
        std::abort();
    }
}

void Device::CreateLogicalDevice()
{   
    detail::QueueFamilyIndices indices = detail::FindQueueFamilies(physical_device_, surface_);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {indices.graphics_family_.value(), indices.present_family_.value()};

    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    create_info.pEnabledFeatures = &device_features;

    create_info.enabledExtensionCount = static_cast<uint32_t>(detail::s_device_extensions.size());
    create_info.ppEnabledExtensionNames = detail::s_device_extensions.data();

    // validation layers
    create_info.enabledLayerCount = static_cast<uint32_t>(detail::s_validation_layers_.size());
    create_info.ppEnabledLayerNames = detail::s_validation_layers_.data();

    if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan Logical device");
    }   

    vkGetDeviceQueue(device_, indices.graphics_family_.value(), 0, &graphics_queue_);
    vkGetDeviceQueue(device_, indices.present_family_.value(), 0, &present_queue_);
}

void Device::CreateCommapdPool()
{
    detail::QueueFamilyIndices indices = detail::FindQueueFamilies(physical_device_, surface_);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = indices.graphics_family_.value();
    pool_info.flags = 
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS)
    {
        std::cerr << "Failed to create Vulkan commang pool!" << std::endl;
        std::abort();
    }
}

// Buffer utility interface
void Device::CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool_;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);
    {
        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
    }
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue_);

    vkFreeCommandBuffers(device_, command_pool_, 1, &command_buffer);
}

VkFormat Device::FindSupportedFormat(const std::vector<VkFormat>& candidates
                            , VkImageTiling tiling
                            , VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        } else if (
            tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

void Device::CreateImageWithInfo(const VkImageCreateInfo &image_info
                        , VkMemoryPropertyFlags properties
                        , VkImage &image
                        , VkDeviceMemory &image_memory)
{
    if (vkCreateImage(device_, &image_info, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(device_, image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    if (vkBindImageMemory(device_, image, image_memory, 0) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to bind image memory!");
    }
}

uint32_t Device::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) 
    {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) 
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");

}

} // namespace renderer
