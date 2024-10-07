#pragma once 

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// renderer includes
#include <renderer/renderer/device.hpp>
#include <renderer/renderer/swap_chain.hpp>
#include <renderer/renderer/vertex_buffer.hpp>


namespace renderer {

class Pipeline
{
public:
    Pipeline(Device& device, VkRenderPass render_pass, VkDescriptorSetLayout descriptor_set_layout);
    ~Pipeline();

    VkPipeline GetGraphicsPipeline() { return graphics_pipeline_;}

private:
    void CreatePipeline(VkRenderPass render_pass, VkDescriptorSetLayout descriptor_set_layout);



    std::vector<char> ReadFile(std::string filename);
    VkShaderModule CreateShaderModule(std::vector<char>& code);
private:
    Device& device_;

    VkPipelineLayout pipeline_layout_;
    VkPipeline graphics_pipeline_;

};

} // namespace renderer