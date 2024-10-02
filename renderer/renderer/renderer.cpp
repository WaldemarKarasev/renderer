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
    VkResult result = swap_chain_.AquireImage(&current_image_index_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return nullptr;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    VkCommandBuffer current_command_buffer = command_buffers_[current_image_index_];

    // vkResetCommandBuffer(current_command_buffer);

    // begin render pass
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(current_command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return current_command_buffer;
}

void Renderer::BeginRenderPass()
{
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = swap_chain_.GetRenderPass();
    render_pass_info.framebuffer = swap_chain_.GetFrameBuffer(current_image_index_);
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_.GetExtent();

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffers_[current_image_index_], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)  swap_chain_.GetExtent().width;
    viewport.height = (float) swap_chain_.GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffers_[current_image_index_], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_.GetExtent();
    vkCmdSetScissor(command_buffers_[current_image_index_], 0, 1, &scissor);
}

void Renderer::Render(VkCommandBuffer command_buffer)
{
    // rendering triangle demo
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.GetGraphicsPipeline());
    vkCmdDraw(command_buffer, 3, 1, 0, 0);
}

void Renderer::EndRenderPass()
{
    // end render pass
    vkCmdEndRenderPass(command_buffers_[current_image_index_]);
}

void Renderer::EndFrame(VkCommandBuffer command_buffer)
{
if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }    

    VkResult result = swap_chain_.SubmitCommandBuffer(command_buffers_[current_image_index_], &current_image_index_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_) {
        framebuffer_resized_ = false;
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    current_image_index_ = (current_image_index_ + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
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

void Renderer::RecreateSwapChain()
{
    swap_chain_.RecreateSwapChain();
}



} // namespace renderer