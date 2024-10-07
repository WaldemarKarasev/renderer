#pragma once 

// vulkan
#include <vulkan/vulkan.h>


// user include
#include <renderer/renderer/device.hpp>


namespace renderer {

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

} // namespace renderer