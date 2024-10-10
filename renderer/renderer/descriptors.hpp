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
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_{};
    };

    DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();

    VkDescriptorSetLayout GetDescriptorSetLayout() { return descriptor_set_layout_; }

private:
    Device& device_;
    VkDescriptorSetLayout descriptor_set_layout_;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_{};

    friend class DescriptorWriter;
};


// =================== Descriptor Pool =================== //
class DescriptorPool
{
public:
    class Builder
    {
    public:
        Builder(Device& device) : device_{device} {}

        Builder& AddPoolSize(VkDescriptorType descriptor_type, uint32_t count);
        Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& SetMaxSets(uint32_t count);
        std::unique_ptr<DescriptorPool> Build(); 

    private:
        Device& device_;
        std::vector<VkDescriptorPoolSize> pool_sizes_{};
        uint32_t max_sets_ = 1000;
        VkDescriptorPoolCreateFlags pool_flags_ = 0;
    };

    DescriptorPool(
        Device& device,
        uint32_t max_sets,
        VkDescriptorPoolCreateFlags pool_flags,
        std::vector<VkDescriptorPoolSize> pool_sizes
    );
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    bool AllocateDescriptor(const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor);
    void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors);

    void ResetPool();
    
private:
    Device& device_;
    VkDescriptorPool descriptor_pool_;

    friend class DescriptorWriter;
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

    friend class DescriptorWriter;
};

// =================== DescriptorWriter =================== //
class DescriptorWriter
{
public:
    DescriptorWriter(DescriptorSetLayout& set_layout, DescriptorPool& pool);

    DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* buffer_info);
    DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* image_info);

    bool Build(VkDescriptorSet& set);
    void Overwrite(VkDescriptorSet& set);

private:
    DescriptorSetLayout& set_layout_;
    DescriptorPool& descriptor_pool_;
    std::vector<VkWriteDescriptorSet> writes_;
};


} // namespace renderer
