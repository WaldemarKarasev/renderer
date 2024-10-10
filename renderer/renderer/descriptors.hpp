#pragma once 

// std
#include <memory>
#include <vector>
#include <unordered_map>

// vulkan
#include <vulkan/vulkan.h>

// user includes
#include <renderer/renderer/device.hpp>
namespace renderer
{

class DescriptorSetLayout
{
public:

    class Builder
    {
    public:
        Builder(Device& device) : device_{device} {};
        Builder& AddBindings(uint32_t binding, 
                             VkDescriptorType descriptor_type, 
                             VkShaderStageFlags stage_flags, 
                             uint32_t count = 1);
        
        std::unique_ptr<DescriptorSetLayout> Build();

    private:
        Device& device_;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBindings> bindings_{};
    }

    DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBindings> bindings);
    ~DescriptorSetLayout();

    VkDescriptorSetLayout GetDescriptorSetLayout() { return descriptor_set_layout_; }

private:
    Device& device_;
    VkDescriptorSetLayout descriptor_set_layout_;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBindings> bindings_{};
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
