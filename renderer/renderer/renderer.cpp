#include <renderer/renderer/renderer.hpp>

#include <stdexcept>

namespace renderer
{

Renderer::Renderer(engine::Window& window)
    : window_{window}
    , device_{window_}
    , swap_chain_{window_, device_}
    , pipeline_{device_, swap_chain_.GetSwapChainInfo()}
{
    CreateCommandBuffers();
}

Renderer::~Renderer()
{

}

VkCommandBuffer Renderer::BeginFrame()
{
    return swap_chain_.BeginFrame();
}

void Renderer::BeginRenderPass()
{
    swap_chain_.BeginRenderPass();
}

void Renderer::Render(VkCommandBuffer command_buffer)
{
    // rendering
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.GetGraphicsPipeline());
    vkCmdDraw(command_buffer, 3, 1, 0, 0);
}

void Renderer::EndRenderPass()
{
    swap_chain_.EndRenderPass();
}

void Renderer::EndFrame()
{
    swap_chain_.EndFrame();
}


void Renderer::CreateCommandBuffers()
{
    command_buffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = device_.GetCommandPool();
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = (uint32_t) command_buffers_.size();

    if (vkAllocateCommandBuffers(device_.GetDevice(), &alloc_info, command_buffers_.data()) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}



} // namespace renderer
