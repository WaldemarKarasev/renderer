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
DescriptorSet::DescriptorSet(Device& device, DescriptorSetLayout layout)
    : device_{device}
{
    CreateDescriptorPool();
    CreateDescriptorBuffers(layout.GetLayout());

}

DescriptorSet::~DescriptorSet()
{

}

void DescriptorSet::CreateDescriptorPool()
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
        throw std::runtime_error("Failed to create descriptor sets pool!");
    }
}

void DescriptorSet::CreateDescriptorBuffers(VkDescriptorSetLayout descriptor_set_layout_)
{
    std::vector<VkDescriptorSetLayout> layouts(SwapChain::MAX_FRAMES_IN_FLIGHT, descriptor_set_layout_);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool_;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT);
    alloc_info.pSetLayouts = layouts.data();

    descriptor_sets_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device_.GetDevice(), &alloc_info, descriptor_sets_.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed allocate descriptor sets!");
    }

    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        // VkDescriptorBufferInfo buffer_info{};
        // buffer_info.buffer = 
    }

}
    
} // namespace renderer
