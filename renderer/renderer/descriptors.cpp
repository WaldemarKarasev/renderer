#include <renderer/renderer/descriptors.hpp>

// std
#include <cassert>
#include <cstdint>
#include <stdexcept>

// renderer includes 
#include <renderer/renderer/swap_chain.hpp>


namespace renderer
{

// ============================== Descriptor Set Layout ============================== 

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBindings(uint32_t binding, 
                    VkDescriptorType descriptor_type, 
                    VkShaderStageFlags stage_flags, 
                    uint32_t count)
{
    assert(binding_.count(binding) == 0 && "Binding already in use");

    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = count;
    layout_binding.stageFlags = stage_flags;
    bindings_[binding] = layout_binding;
    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build()
{
    return std::make_unique<DescriptorSetLayout>(device_, std::move(bindings_));
}


DescriptorSetLayout::DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBindings> bindings)
    : device_{device}
    , bindings_{std::move(bindings)}
{
    std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
    for (auto kv : binding)
    {
        set_layout_bindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingsCount = static_cast<uint32_t>(set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = set_layout_bindings.data();

    if (vkDescriptorSetLayout(device_.GetDevice(), &descriptor_set_layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

}
DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(device.GetDevice(), descriptor_set_layout_, nullptr);
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
