#pragma once 

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// renderer includes
#include <renderer/renderer/device.hpp>
#include <renderer/renderer/swap_chain.hpp>

namespace renderer {

class Pipeline
{
public:
    Pipeline(Device& device, SwapChain& swap_chain);
    ~Pipeline();

    VkPipeline GetGraphicsPipiline() { return graphics_pipeline_;}

private:
    void CreatePipeline();

    std::vector<char> ReadFile(std::string filname);
    VkShaderModule CreateShaderModule(std::vector<char>& code);
private:
    Device& device_;
    SwapChain& swap_chain_;

    VkRenderPass render_pass_;
    VkPipelineLayout pipeline_layout_;
    VkPipeline graphics_pipeline_;

};

} // namespace renderer