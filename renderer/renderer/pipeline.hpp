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
    Pipeline(Device& device);
    ~Pipeline();

private:
    void CreatePipeline();

    std::vector<char> ReadFile(std::string filname);
    VkShaderModule CreateShaderModule(std::vector<char>& code);

private:
    Device& device_;
};

} // namespace renderer