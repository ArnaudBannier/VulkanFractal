/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_renderer.hpp"
#include "vulkan/ve_tools.hpp"

Renderer::Renderer(
    vk::PhysicalDevice &physicalDevice,
    vk::Device &device,
    vk::SurfaceKHR surface,
    vk::Extent2D windowExtent,
    uint32_t graphicsQueueFamilyIndex,
    uint32_t presentQueueFamilyIndex,
    vk::CommandPool commandPool
)
    : m_physicalDevice{ physicalDevice }
    , m_device{ device }
    , m_surface{ surface }
    , m_presentQueueFamilyIndex{ presentQueueFamilyIndex }
    , m_graphicsQueueFamilyIndex{ graphicsQueueFamilyIndex }
    , m_frameIndex{ 0 }
    , m_commandPool{ commandPool }
    , m_isFrameStarted{ false }
{
    init(windowExtent);
}

Renderer::Renderer(VulkanBase &base, vk::Extent2D windowExtent)
    : m_physicalDevice{ base.getPhysicalDevice() }
    , m_device{ base.getDevice() }
    , m_surface{ base.getSurface() }
    , m_presentQueueFamilyIndex{ base.getPresentQueueFamilyIndex() }
    , m_graphicsQueueFamilyIndex{ base.getGraphicsQueueFamilyIndex() }
    , m_frameIndex{ 0 }
    , m_commandPool{ base.getCommandPool() }
    , m_isFrameStarted{ false }
{
    init(windowExtent);
}

void Renderer::init(vk::Extent2D windowExtent)
{
    createSwapchain(windowExtent, VK_NULL_HANDLE);
    createImageViews();
    createDepthResources();
    createRenderPass();
    createFramebuffers();
    createSyncObjects();
    createCommandBuffers();
}

Renderer::~Renderer()
{
    std::cout << "Free command buffers" << std::endl;
    m_device.freeCommandBuffers(m_commandPool, m_commandBuffers);
    m_commandBuffers.clear();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_device.destroySemaphore(m_renderFinishedSemaphores[i]);
        m_device.destroySemaphore(m_imageAvailableSemaphores[i]);
        m_device.destroyFence(m_inFlightFences[i]);
    }

    m_device.destroyRenderPass(m_renderPass);

    for (uint32_t i = 0; i < m_imageCount; i++)
    {
        m_device.destroyFramebuffer(m_framebuffers[i]);
        m_device.destroyImageView(m_imageViews[i]);
        m_device.destroyImageView(m_depthImageViews[i]);
        m_device.destroyImage(m_depthImages[i]);
        m_device.freeMemory(m_depthImageMemories[i]);
    }
    m_framebuffers.clear();
    m_imageViews.clear();
    m_images.clear();
    m_depthImageViews.clear();
    m_depthImages.clear();
    m_depthImageMemories.clear();

    m_device.destroySwapchainKHR(m_swapchain);
}

VkCommandBuffer Renderer::beginFrame()
{
    assert(!m_isFrameStarted && "Can't call beginFrame while already in progress");

    vk::Result result = m_device.waitForFences(
        1, &m_inFlightFences[m_frameIndex],
        VK_TRUE, std::numeric_limits<uint64_t>::max());

    result = m_device.acquireNextImageKHR(
        m_swapchain, std::numeric_limits<uint64_t>::max(),
        m_imageAvailableSemaphores[m_frameIndex],
        VK_NULL_HANDLE,
        &m_imageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        return nullptr;
    }
    else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
    {
        throw std::runtime_error("failed to acquire swapchain image!");
        return nullptr;
    }

    m_isFrameStarted = true;

    vk::CommandBuffer commandBuffer = m_commandBuffers[m_frameIndex];
    commandBuffer.reset();

    vk::CommandBufferBeginInfo beginInfo{};
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void Renderer::endFrame()
{
    assert(m_isFrameStarted && "Can't call endFrame while frame is not in progress");

    vk::CommandBuffer commandBuffer = m_commandBuffers[m_frameIndex];
    commandBuffer.end();

    vk::Queue graphicsQueue = m_device.getQueue(m_graphicsQueueFamilyIndex, 0);
    vk::Queue presentQueue = m_device.getQueue(m_presentQueueFamilyIndex, 0);

    // Submit
    vk::SubmitInfo submitInfo = {};
    vk::Semaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_frameIndex] };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_frameIndex];

    vk::Semaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_frameIndex] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vk::Result result = m_device.resetFences(1, &m_inFlightFences[m_frameIndex]);
    result = graphicsQueue.submit(1, &submitInfo, m_inFlightFences[m_frameIndex]);

    // Present
    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = { m_swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_imageIndex;

    result = presentQueue.presentKHR(presentInfo);

    m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    m_isFrameStarted = false;
}

void Renderer::beginRenderPass()
{
    assert(
        m_isFrameStarted &&
        "Can't call beginSwapChainRenderPass if frame is not in progress");

    vk::CommandBuffer commandBuffer = m_commandBuffers[m_frameIndex];

    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.renderPass = m_renderPass;
    renderPassBeginInfo.framebuffer = m_framebuffers[m_imageIndex];
    renderPassBeginInfo.renderArea.setOffset({ 0, 0 });
    renderPassBeginInfo.renderArea.extent = m_extent;

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].setColor({ 0.005f, 0.005f, 0.005f, 1.0f });
    clearValues[1].setDepthStencil({ 1.0f, 0 });

    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_extent.width);
    viewport.height = static_cast<float>(m_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    commandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.setOffset({ 0, 0 });
    scissor.extent = m_extent;
    commandBuffer.setScissor(0, 1, &scissor);
}

void Renderer::endRenderPass()
{
    assert(
        m_isFrameStarted &&
        "Can't call endSwapChainRenderPass if frame is not in progress");

    vk::CommandBuffer commandBuffer = m_commandBuffers[m_frameIndex];
    commandBuffer.endRenderPass();
}

void Renderer::recreateSwapchain(vk::Extent2D extent)
{
    m_device.waitIdle();

    std::cout << "Recreate Swapchain ("
        << extent.width << ", "
        << extent.height << ")" << std::endl;

    for (uint32_t i = 0; i < m_imageCount; i++)
    {
        m_device.destroyFramebuffer(m_framebuffers[i]);
        m_device.destroyImageView(m_imageViews[i]);
        m_device.destroyImageView(m_depthImageViews[i]);
        m_device.destroyImage(m_depthImages[i]);
        m_device.freeMemory(m_depthImageMemories[i]);
    }
    m_framebuffers.clear();
    m_imageViews.clear();
    m_images.clear();
    m_depthImageViews.clear();
    m_depthImages.clear();
    m_depthImageMemories.clear();

    vk::SwapchainKHR oldSwapchain = m_swapchain;
    createSwapchain(extent, m_swapchain);

    m_device.destroySwapchainKHR(oldSwapchain);

    createImageViews();
    createDepthResources();
    createFramebuffers();
}

void Renderer::createSwapchain(vk::Extent2D windowExtent, vk::SwapchainKHR oldSwapchain)
{
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
    vk::SurfaceFormatKHR surfaceFormat = chooseSurfaceFormat();
    vk::PresentModeKHR presentMode = choosePresentMode();
    vk::Extent2D extent = chooseExtent(surfaceCapabilities, windowExtent);

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapchainCI{};
    swapchainCI.surface = m_surface;
    swapchainCI.minImageCount = imageCount;
    swapchainCI.imageFormat = surfaceFormat.format;
    swapchainCI.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCI.imageExtent = extent;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    uint32_t queueFamilyIndices[] = { m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex };
    if (m_graphicsQueueFamilyIndex != m_presentQueueFamilyIndex)
    {
        swapchainCI.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCI.queueFamilyIndexCount = 2;
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainCI.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.pQueueFamilyIndices = nullptr;
    }

    swapchainCI.preTransform = surfaceCapabilities.currentTransform;
    swapchainCI.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCI.presentMode = presentMode;
    swapchainCI.clipped = vk::True;
    swapchainCI.oldSwapchain = oldSwapchain;

    m_swapchain = m_device.createSwapchainKHR(swapchainCI);
    m_images = m_device.getSwapchainImagesKHR(m_swapchain);
    m_imageCount = static_cast<uint32_t>(m_images.size());

    m_imageFormat = surfaceFormat.format;
    m_extent = extent;
}

void Renderer::createImageViews()
{
    m_imageViews.resize(m_imageCount);

    for (uint32_t i = 0; i < m_imageCount; i++)
    {
        vk::ImageViewCreateInfo imageViewCI{};
        imageViewCI.image = m_images[i];
        imageViewCI.viewType = vk::ImageViewType::e2D;
        imageViewCI.format = m_imageFormat;
        imageViewCI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageViewCI.subresourceRange.baseMipLevel = 0;
        imageViewCI.subresourceRange.levelCount = 1;
        imageViewCI.subresourceRange.baseArrayLayer = 0;
        imageViewCI.subresourceRange.layerCount = 1;

        m_imageViews[i] = m_device.createImageView(imageViewCI);
    }
}

void Renderer::createDepthResources()
{
    m_depthFormat = vk::Format::eD32Sfloat;

    m_depthImages.resize(m_imageCount);
    m_depthImageMemories.resize(m_imageCount);
    m_depthImageViews.resize(m_imageCount);

    for (int i = 0; i < m_depthImages.size(); i++)
    {
        // Create depth image
        vk::ImageCreateInfo imageCI{};
        imageCI.imageType = vk::ImageType::e2D;
        imageCI.extent.width = m_extent.width;
        imageCI.extent.height = m_extent.height;
        imageCI.extent.depth = 1;
        imageCI.mipLevels = 1;
        imageCI.arrayLayers = 1;
        imageCI.format = m_depthFormat;
        imageCI.tiling = vk::ImageTiling::eOptimal;
        imageCI.initialLayout = vk::ImageLayout::eUndefined;
        imageCI.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        imageCI.samples = vk::SampleCountFlagBits::e1;
        imageCI.sharingMode = vk::SharingMode::eExclusive;

        m_depthImages[i] = m_device.createImage(imageCI);

        // Create memory and bind it
        vk::MemoryRequirements memoryRequirements =
            m_device.getImageMemoryRequirements(m_depthImages[i]);
        vk::PhysicalDeviceMemoryProperties memoryProperties =
            m_physicalDevice.getMemoryProperties();

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = tools::findMemoryTypeIndex(
            memoryProperties,
            memoryRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eDeviceLocal);

        m_depthImageMemories[i] = m_device.allocateMemory(allocInfo);
        m_device.bindImageMemory(m_depthImages[i], m_depthImageMemories[i], 0);

        // Create image view
        vk::ImageViewCreateInfo imageViewCI{};
        imageViewCI.image = m_depthImages[i];
        imageViewCI.viewType = vk::ImageViewType::e2D;
        imageViewCI.format = m_depthFormat;
        imageViewCI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        imageViewCI.subresourceRange.baseMipLevel = 0;
        imageViewCI.subresourceRange.levelCount = 1;
        imageViewCI.subresourceRange.baseArrayLayer = 0;
        imageViewCI.subresourceRange.layerCount = 1;

        m_depthImageViews[i] = m_device.createImageView(imageViewCI);
    }
}

void Renderer::createRenderPass()
{
    RenderPassBuilder renderPassBuilder;

    // Color attachement [0]
    renderPassBuilder.attachmentBegin(m_imageFormat)
        .attachmentLoadOp(vk::AttachmentLoadOp::eClear)
        .attachmentStoreOp(vk::AttachmentStoreOp::eStore)
        .attachmentStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .attachmentStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .attachmentInitialLayout(vk::ImageLayout::eUndefined)
        .attachmentFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    // Depth attachement [1]
    renderPassBuilder.attachmentBegin(m_depthFormat)
        .attachmentLoadOp(vk::AttachmentLoadOp::eClear)
        .attachmentStoreOp(vk::AttachmentStoreOp::eDontCare)
        .attachmentStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .attachmentStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .attachmentFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // Subpass using the two attachments
    renderPassBuilder.subpassBegin(vk::PipelineBindPoint::eGraphics)
        .subpassColorAttachment(vk::ImageLayout::eColorAttachmentOptimal, 0)
        .subpassDepthStencilAttachment(vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);

    renderPassBuilder.dependencyBegin(VK_SUBPASS_EXTERNAL, 0)
        .dependencySrcStageMask(
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .dependencyDstStageMask(
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .dependencySrcAccessMask(
            vk::AccessFlagBits::eNone)
        .dependencyDstAccessMask(
            vk::AccessFlagBits::eColorAttachmentWrite |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    m_renderPass = renderPassBuilder.build(m_device);
}

void Renderer::createFramebuffers()
{
    m_framebuffers.resize(m_imageCount);
    for (size_t i = 0; i < m_imageCount; i++)
    {
        std::array<vk::ImageView, 2> attachments = {
            m_imageViews[i],
            m_depthImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferCI{};
        framebufferCI.renderPass = m_renderPass;
        framebufferCI.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCI.pAttachments = attachments.data();
        framebufferCI.width = m_extent.width;
        framebufferCI.height = m_extent.height;
        framebufferCI.layers = 1;

        m_framebuffers[i] = m_device.createFramebuffer(framebufferCI);
    }
}

void Renderer::createSyncObjects()
{
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphoreCI{};

    vk::FenceCreateInfo fenceCI{};
    fenceCI.flags = vk::FenceCreateFlagBits::eSignaled;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        m_imageAvailableSemaphores[i] = m_device.createSemaphore(semaphoreCI);
        m_renderFinishedSemaphores[i] = m_device.createSemaphore(semaphoreCI);
        m_inFlightFences[i] = m_device.createFence(fenceCI);
    }
}

void Renderer::createCommandBuffers()
{
    vk::CommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocInfo.commandPool = m_commandPool;
    commandBufferAllocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    m_commandBuffers = m_device.allocateCommandBuffers(commandBufferAllocInfo);
}

vk::SurfaceFormatKHR Renderer::chooseSurfaceFormat()
{
    std::vector<vk::SurfaceFormatKHR> availableFormats = m_physicalDevice.getSurfaceFormatsKHR(m_surface);

    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR Renderer::choosePresentMode()
{
    std::vector<vk::PresentModeKHR> availablePresentModes = m_physicalDevice.getSurfacePresentModesKHR(m_surface);
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eFifo)
        {
            std::cout << "Present mode: " << vk::to_string(availablePresentMode) << std::endl;

            return availablePresentMode;
        }
    }

    std::cout << "Present mode: " << vk::to_string(availablePresentModes[0]) << std::endl;
    return availablePresentModes[0];
}

vk::Extent2D Renderer::chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities, vk::Extent2D windowExtent)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = windowExtent;

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

float Renderer::getAspectRatio() const
{
    return static_cast<float>(m_extent.width) / static_cast<float>(m_extent.height);
}

RenderPassBuilder &RenderPassBuilder::attachmentBegin(vk::Format format)
{
    vk::AttachmentDescription attachementDescription{};
    attachementDescription.format = format;

    m_attachmentDescriptions.push_back(attachementDescription);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentFlags(vk::AttachmentDescriptionFlags flags)
{
    m_attachmentDescriptions.back().setFlags(flags);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentFormat(vk::Format format)
{
    m_attachmentDescriptions.back().setFormat(format);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentSamples(vk::SampleCountFlagBits samples)
{
    m_attachmentDescriptions.back().setSamples(samples);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentLoadOp(vk::AttachmentLoadOp loadOp)
{
    m_attachmentDescriptions.back().setLoadOp(loadOp);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentStoreOp(vk::AttachmentStoreOp StoreOp)
{
    m_attachmentDescriptions.back().setStoreOp(StoreOp);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentStencilLoadOp(vk::AttachmentLoadOp stencilLoadOp)
{
    m_attachmentDescriptions.back().setStencilLoadOp(stencilLoadOp);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentStencilStoreOp(vk::AttachmentStoreOp stencilStoreOp)
{
    m_attachmentDescriptions.back().setStencilStoreOp(stencilStoreOp);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentInitialLayout(vk::ImageLayout initialLayout)
{
    m_attachmentDescriptions.back().setInitialLayout(initialLayout);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::attachmentFinalLayout(vk::ImageLayout finalLayout)
{
    m_attachmentDescriptions.back().setFinalLayout(finalLayout);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::subpassBegin(vk::PipelineBindPoint pipelineBindPoint)
{
    vk::SubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = pipelineBindPoint;
    m_subpassDescriptions.push_back(subpassDescription);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::subpassColorAttachment(vk::ImageLayout imageLayout, uint32_t attachment)
{
    vk::SubpassDescription &subpassDescription = m_subpassDescriptions.back();
    vk::AttachmentReference attachmentReference{};
    attachmentReference.layout = imageLayout;
    attachmentReference.attachment = attachment;

    m_colorAttachmentReferences.push_back(attachmentReference);
    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(m_colorAttachmentReferences.size());
    subpassDescription.pColorAttachments = m_colorAttachmentReferences.data();
    return *this;
}

RenderPassBuilder &RenderPassBuilder::subpassDepthStencilAttachment(vk::ImageLayout imageLayout, uint32_t attachment)
{
    vk::SubpassDescription &subpassDescription = m_subpassDescriptions.back();
    m_depthAttachmentReference.layout = imageLayout;
    m_depthAttachmentReference.attachment = attachment;
    subpassDescription.pDepthStencilAttachment = &m_depthAttachmentReference;
    return *this;
}

RenderPassBuilder &RenderPassBuilder::dependencyBegin(uint32_t srcSubpass, uint32_t dstSubpass)
{
    vk::SubpassDependency subpassDepedency{};
    subpassDepedency.srcSubpass = srcSubpass;
    subpassDepedency.dstSubpass = dstSubpass;
    m_subpassDependencies.push_back(subpassDepedency);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::dependencySrcStageMask(vk::PipelineStageFlags srcStageMask)
{
    m_subpassDependencies.back().setSrcStageMask(srcStageMask);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::dependencyDstStageMask(vk::PipelineStageFlags dstStageMask)
{
    m_subpassDependencies.back().setDstStageMask(dstStageMask);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::dependencySrcAccessMask(vk::AccessFlags srcAccessMask)
{
    m_subpassDependencies.back().setSrcAccessMask(srcAccessMask);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::dependencyDstAccessMask(vk::AccessFlags dstAccessMask)
{
    m_subpassDependencies.back().setDstAccessMask(dstAccessMask);
    return *this;
}

RenderPassBuilder &RenderPassBuilder::dependencyFlags(vk::DependencyFlags flags)
{
    m_subpassDependencies.back().setDependencyFlags(flags);
    return *this;
}

vk::RenderPass RenderPassBuilder::build(const vk::Device &device) const
{
    vk::RenderPassCreateInfo renderPassCI{};
    renderPassCI.attachmentCount = static_cast<uint32_t>(m_attachmentDescriptions.size());
    renderPassCI.pAttachments = m_attachmentDescriptions.data();
    renderPassCI.subpassCount = static_cast<uint32_t>(m_subpassDescriptions.size());
    renderPassCI.pSubpasses = m_subpassDescriptions.data();
    renderPassCI.dependencyCount = static_cast<uint32_t>(m_subpassDependencies.size());
    renderPassCI.pDependencies = m_subpassDependencies.data();
    return device.createRenderPass(renderPassCI);
}

vk::UniqueRenderPass RenderPassBuilder::buildUnique(const vk::Device &device) const
{
    vk::RenderPassCreateInfo renderPassCI{};
    renderPassCI.attachmentCount = static_cast<uint32_t>(m_attachmentDescriptions.size());
    renderPassCI.pAttachments = m_attachmentDescriptions.data();
    renderPassCI.subpassCount = static_cast<uint32_t>(m_subpassDescriptions.size());
    renderPassCI.pSubpasses = m_subpassDescriptions.data();
    renderPassCI.dependencyCount = static_cast<uint32_t>(m_subpassDependencies.size());
    renderPassCI.pDependencies = m_subpassDependencies.data();
    return device.createRenderPassUnique(renderPassCI);
}
