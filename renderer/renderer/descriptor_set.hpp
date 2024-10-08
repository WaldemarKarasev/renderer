#pragma once 

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// user includes
#include <renderer/renderer/device.hpp>
#include <renderer/renderer/descriptor_set_layout.hpp>

namespace renderer
{

class DescriptorSet
{
public:
    DescriptorSet(Device& device, DescriptorSetLayout layout);
    ~DescriptorSet();

private:
    void CreateDescriptorPool();
    void CreateDescriptorBuffers(VkDescriptorSetLayout descriptor_set_layout_);

private:
    Device& device_;

    VkDescriptorPool descriptor_pool_;
    std::vector<VkDescriptorSet> descriptor_sets_;
};

} // namespace renderer
