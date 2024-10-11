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
    // assert(binding_.count(binding) == 0 && "Binding already in use");

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


DescriptorSetLayout::DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : device_{device}
    , bindings_{std::move(bindings)}
{
    std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
    for (auto kv : bindings_)
    {
        set_layout_bindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
    descriptor_set_layout_info.pBindings = set_layout_bindings.data();

    if (vkCreateDescriptorSetLayout(device_.GetDevice(), &descriptor_set_layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

}
DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(device_.GetDevice(), descriptor_set_layout_, nullptr);
}

// =================== Descriptor Pool =================== //

DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptor_type, uint32_t count)
{
    pool_sizes_.push_back({descriptor_type, count});
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
{
    pool_flags_ = flags;
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count)
{
    max_sets_ = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build()
{
    return std::make_unique<DescriptorPool>(device_, max_sets_, pool_flags_, std::move(pool_sizes_));
}


DescriptorPool::DescriptorPool(
    Device& device,
    uint32_t max_sets,
    VkDescriptorPoolCreateFlags pool_flags,
    std::vector<VkDescriptorPoolSize> pool_sizes)
    : device_{device}
{
    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    descriptor_pool_info.pPoolSizes = pool_sizes.data();
    descriptor_pool_info.maxSets = max_sets;
    descriptor_pool_info.flags = pool_flags;

    if (vkCreateDescriptorPool(device_.GetDevice(), &descriptor_pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }    
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(device_.GetDevice(), descriptor_pool_, nullptr);
}

bool DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor)
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool_;
    alloc_info.pSetLayouts = &descriptor_set_layout;
    alloc_info.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(device_.GetDevice(), &alloc_info, &descriptor) != VK_SUCCESS) 
    {
        return false;
    }
    return true;
}

void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors)
{
    vkFreeDescriptorSets(device_.GetDevice(), descriptor_pool_, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void DescriptorPool::ResetPool()
{
    vkResetDescriptorPool(device_.GetDevice(), descriptor_pool_, 0);

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

// =================== DescriptorWriter =================== //
DescriptorWriter::DescriptorWriter(DescriptorSetLayout& set_layout, DescriptorPool& pool)
    : set_layout_{set_layout}
    , descriptor_pool_{pool} {}

DescriptorWriter& DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* buffer_info)
{
    assert(set_layout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");
    
    auto& binding_description = set_layout_.bindings_[binding];

    assert(
        binding_description.descriptorCount == 1 && 
        "Binding single descriptor info, but binding expects multiple");
    
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = binding_description.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = buffer_info;
    write.descriptorCount = 1;
    
    writes_.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* image_info)
{
    assert(set_layout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

    auto& binding_description = set_layout_.bindings_[binding];

    assert(
        binding_description.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = binding_description.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = image_info;
    write.descriptorCount = 1;

    writes_.push_back(write);
    return *this;
}

bool DescriptorWriter::Build(VkDescriptorSet& set)
{
    bool success = descriptor_pool_.AllocateDescriptor(set_layout_.GetDescriptorSetLayout(), set);
    if (!success) 
    {
        return false;
    }
    Overwrite(set);
    return true;
}

void DescriptorWriter::Overwrite(VkDescriptorSet& set)
{
    for (auto& write : writes_)
    {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(descriptor_pool_.device_.GetDevice(), writes_.size(), writes_.data(), 0, nullptr);

}


   
} // namespace renderer
