#include <renderer/renderer/renderer.hpp>


namespace renderer
{

Renderer::Renderer(engine::Window& window)
    : device_{window_}
    , swap_chain_{window_, device_}
    , pipeline_{device_, swap_chain_}
{
    CreateFramebuffers();
    CreateCommapdPool();
    CreateCommandBuffers();
    CreateSyncObjects();
}

Renderer::~Renderer()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device_.GetDevice(), render_finished_semaphores_[i], nullptr);
        vkDestroySemaphore(device_.GetDevice(), image_available_semaphores_[i], nullptr);
        vkDestroyFence(device_.GetDevice(), in_flight_fences_[i], nullptr);
    }
    vkDestroyCommandPool(device_.GetDevice(), command_pool_, nullptr);
}

void Renderer::DrawFrame()
{
    vkWaitForFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(device_.GetDevice(), swap_chain_.GetSwapChain(), UINT64_MAX, image_available_semaphores_[current_frame_], VL_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    vkResetFence(device_.GetDevice(), 1, &in_flight_fences_[current_frame_]);

    vkResetCommandBuffer(command_buffers_[current_frame_]);
    RecordCommandBuffer(command_buffers_[current_frame_], image_index);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {image_available_semaphores_[current_frame_]};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = wait_semaphores;
    submitInfo.pWaitDstStageMask = wait_stages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffers_[current_frame_];

    VkSemaphore signal_semaphores[] = {render_finished_semaphores_[current_frame_]};

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(device_.GetGraphicsQueue(), 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS) {        
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR present_info{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = {swap_chain_.GetSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swap_chains;

    presentInfo.pImageIndices = &image_index;

    result = vkQueuePresentKHR(device_.GetPresentQueue(), &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_) {
        framebuffer_resized_ = false;
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}


void Renderer::CreateFramebuffers()
{
    auto image_views = swap_chain_.GetImageViews();
    swap_chain_framebuffers_.resize(image_views.size());

    for (size_t i = 0; i < image_views.size(); ++i)
    {
        VkImageView attachments[] = {image_views[i]};

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = renderPass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = swapChainExtent.width;
        framebuffer_info.height = swapChainExtent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFrameBuffer(device_.GetDevice(), &frame_buffer_info, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void Renderer::CreateCommapdPool()
{
    detail::QueueFamilyIndices indices = detail::FindQueueFamilies(device_.GetPhysicalDevice(), device_.GetSurface());

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = indices.graphics_family_.value();
    pool_info.flags = 
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    if (vkCreateCommandPool(device_.GetDevice(), &pool_info, nullptr, &command_pool_) != VK_SUCCESS)
    {
        std::cerr << "Failed to create Vulkan commang pool!" << std::endl;
        std::abort();
    }
}

void Renderer::CreateCommandBuffers()
{
    command_buffers_.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, command_buffers_.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void Renderer::CreateSyncObjects()
{
    image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device_.GetDevice(), &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void Renderer::RecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index)
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = renderPass;
    render_pass_info.framebuffer = swap_chain_framebuffers_[image_index];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_.GetExtent();

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffers_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.GetGraphicsPipiline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swap_chain_.GetExtent().width;
    viewport.height = (float) swap_chain_.GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_.GetExtent();
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }    
}

void Renderer::RecreateSwapChain()
{
    swap_chain_.RecreateSwapChain();
    CreateFramebuffers();
}


} // namespace renderer
