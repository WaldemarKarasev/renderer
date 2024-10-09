#include <renderer/renderer/descriptors.hpp>

// std
#include <cstdint>
#include <stdexcept>

// renderer includes 
#include <renderer/renderer/swap_chain.hpp>


namespace renderer
{

// ============================== Descriptor Set Layout ============================== 
DescriptorSetLayout::DescriptorSetLayout(Device& device)
    : device_{device}
{
    CreateDescriptorSetLayout();

}

DescriptorSetLayout::~DescriptorSetLayout()
{

}


void DescriptorSetLayout::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding ubo_layout_binding{};
    ubo_layout_binding.binding = 0;
    ubo_layout_binding.descriptorCount = 1;
    ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layout_binding.pImmutableSamplers = nullptr;
    ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &ubo_layout_binding;

    if (vkCreateDescriptorSetLayout(device_.GetDevice(), &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

// ============================== Descriptor Set ============================== 
DescriptorSet::DescriptorSet(Device& device, VkDescriptorSetLayout layout, VkDescriptorPool pool)
    : device_{device}
{
    CreateDescriptorSet(layout, pool);
}

DescriptorSet::~DescriptorSet()
{
    
}

void DescriptorSet::CreateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool)
{

}

   
} // namespace renderer
