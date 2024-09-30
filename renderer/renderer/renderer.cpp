#include <renderer/renderer/renderer.hpp>


namespace renderer
{

Renderer::Renderer(engine::Window& window)
    : device_{window_}
    , swap_chain_{window_, device_}
    , pipeline_{device_, swap_chain_}
{

}

Renderer::~Renderer()
{

}

void Renderer::BeginFrame()
{
    swap_chain_.BeginFrame();
}

VkCommandBuffer Renderer::BeginRenderPass()
{
    return swap_chain_.BeginRenderPass();
}

void Renderer::Render(VkCommandBuffer command_buffer)
{
    // rendering
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.GetGraphicsPipiline());
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



} // namespace renderer
