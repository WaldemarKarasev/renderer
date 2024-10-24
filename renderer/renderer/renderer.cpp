#include <renderer/renderer/renderer.hpp>

// std
#include <array>
#include <stdexcept>
#include <iostream>

namespace renderer
{

Renderer::Renderer(Device& device, engine::Window& window)
    : window_{window}
    , device_{device}
    , swap_chain_{device_, window_.GetExtent()}
    , current_image_index_{0}
    , current_frame_index_{0}
{
    CreateCommandBuffers();
}

Renderer::~Renderer()
{

}

VkCommandBuffer Renderer::BeginFrame()
{
    VkResult result = swap_chain_.AcquireImage(&current_image_index_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return nullptr;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }
    VkCommandBuffer current_command_buffer = command_buffers_[current_frame_index_];

    // vkResetCommandBuffer(current_command_buffer, 0);

    // Begin command buffer
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(current_command_buffer, &begin_info) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return current_command_buffer;
}

void Renderer::BeginRenderPass(VkCommandBuffer command_buffer)
{
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = swap_chain_.GetRenderPass();
    render_pass_info.framebuffer = swap_chain_.GetFrameBuffer(current_image_index_);
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_.GetExtent();


    std::array<VkClearValue, 2> clear_values{};
    
    clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_values[1].depthStencil = {1.0f, 0};
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap_chain_.GetExtent().width);
    viewport.height = static_cast<float>(swap_chain_.GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_.GetExtent();
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void Renderer::EndRenderPass(VkCommandBuffer command_buffer)
{
    // end render pass
    vkCmdEndRenderPass(command_buffer);
}

void Renderer::EndFrame(VkCommandBuffer command_buffer)
{
if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }    

    VkResult result = swap_chain_.SubmitCommandBuffer(command_buffer, &current_image_index_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_.WasResized()) {
        window_.ResetResizedFlag();
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    current_frame_index_ = (current_frame_index_ + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
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
    int width = 0;
    int height = 0;
    VkExtent2D extent;
    while (extent.width == 0 || extent.height == 0)
    {
        extent = window_.GetExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device_.GetDevice());
    
    swap_chain_.RecreateSwapChain(extent);
}



} // namespace renderer
