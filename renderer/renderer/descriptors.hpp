#pragma once 

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// user includes
#include <renderer/renderer/device.hpp>
namespace renderer
{

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(Device& device);
    ~DescriptorSetLayout();

    VkDescriptorSetLayout GetLayout() { return descriptor_set_layout_; }

private:

    void CreateDescriptorSetLayout();


private:
    Device& device_;
    VkDescriptorSetLayout descriptor_set_layout_;
};

class DescriptorPool
{
public:
    DescriptorPool(Device& device)
        : device_{device}
    {
        CreateDescriptorPool();
    }

    VkDescriptorPool GetPool() { return descriptor_pool_; }

private:
    void CreateDescriptorPool()
    {
        VkDescriptorPoolSize pool_size{};
        pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size.descriptorCount = static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = 1;
        pool_info.pPoolSizes = &pool_size;
        pool_info.maxSets = static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(device_.GetDevice(), &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }


private:
    Device& device_;
    VkDescriptorPool descriptor_pool_;
};

class DescriptorSet
{
public:
    DescriptorSet(Device& device, VkDescriptorSetLayout layout, VkDescriptorPool pool);
    ~DescriptorSet();

private:
    void CreateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool);

private:
    Device& device_;

    VkDescriptorSet descriptor_set_;
};

} // namespace renderer
