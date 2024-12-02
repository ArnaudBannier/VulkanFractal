/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "ve_base.hpp"

class RenderPassBuilder
{
public:
    RenderPassBuilder() {};

    RenderPassBuilder &attachmentBegin(vk::Format format);
    RenderPassBuilder &attachmentFlags(vk::AttachmentDescriptionFlags flags);
    RenderPassBuilder &attachmentFormat(vk::Format format);
    RenderPassBuilder &attachmentSamples(vk::SampleCountFlagBits samples);
    RenderPassBuilder &attachmentLoadOp(vk::AttachmentLoadOp loadOp);
    RenderPassBuilder &attachmentStoreOp(vk::AttachmentStoreOp StoreOp);
    RenderPassBuilder &attachmentStencilLoadOp(vk::AttachmentLoadOp stencilLoadOp);
    RenderPassBuilder &attachmentStencilStoreOp(vk::AttachmentStoreOp stencilStoreOp);
    RenderPassBuilder &attachmentInitialLayout(vk::ImageLayout initialLayout);
    RenderPassBuilder &attachmentFinalLayout(vk::ImageLayout finalLayout);

    RenderPassBuilder &subpassBegin(vk::PipelineBindPoint pipelineBindPoint);
    RenderPassBuilder &subpassColorAttachment(vk::ImageLayout imageLayout, uint32_t attachment);
    RenderPassBuilder &subpassDepthStencilAttachment(vk::ImageLayout imageLayout, uint32_t attachment);

    RenderPassBuilder &dependencyBegin(uint32_t srcSubpass, uint32_t dstSubpass);
    RenderPassBuilder &dependencySrcStageMask(vk::PipelineStageFlags srcStageMask);
    RenderPassBuilder &dependencyDstStageMask(vk::PipelineStageFlags dstStageMask);
    RenderPassBuilder &dependencySrcAccessMask(vk::AccessFlags srcAccessMask);
    RenderPassBuilder &dependencyDstAccessMask(vk::AccessFlags dstAccessMask);
    RenderPassBuilder &dependencyFlags(vk::DependencyFlags flags);

    vk::RenderPass build(const vk::Device &device) const;
    vk::UniqueRenderPass buildUnique(const vk::Device &device) const;

private:
    std::vector<vk::AttachmentDescription> m_attachmentDescriptions;
    std::vector<vk::SubpassDescription> m_subpassDescriptions;
    std::vector<vk::SubpassDependency> m_subpassDependencies;
    std::vector<vk::AttachmentReference> m_colorAttachmentReferences;
    vk::AttachmentReference m_depthAttachmentReference;
};

class Renderer
{
public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    Renderer(
        vk::PhysicalDevice &physicalDevice,
        vk::Device &device,
        vk::SurfaceKHR surface,
        vk::Extent2D windowExtent,
        uint32_t graphicsQueueFamilyIndex,
        uint32_t presentQueueFamilyIndex,
        vk::CommandPool commandPool
    );
    Renderer(VulkanBase &base, vk::Extent2D windowExtent);

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;
    ~Renderer();

    vk::Extent2D getExtent() const { return m_extent; }
    uint32_t getWidth() const { return m_extent.width; }
    uint32_t getHeight() const { return m_extent.height; }
    float getAspectRatio() const;

    uint32_t getFrameIndex() const { return m_frameIndex; }
    vk::RenderPass getRenderPass() const { return m_renderPass; }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginRenderPass();
    void endRenderPass();

    void recreateSwapchain(vk::Extent2D extent);

private:
    void init(vk::Extent2D windowExtent);
    void createSwapchain(vk::Extent2D windowExtent, vk::SwapchainKHR oldSwapchain);
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();
    void createCommandBuffers();

    vk::SurfaceFormatKHR chooseSurfaceFormat();
    vk::PresentModeKHR choosePresentMode();
    vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities, vk::Extent2D windowExtent);

    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_device;
    vk::SurfaceKHR m_surface;
    uint32_t m_graphicsQueueFamilyIndex;
    uint32_t m_presentQueueFamilyIndex;
    vk::CommandPool m_commandPool;

    vk::SwapchainKHR m_swapchain;
    vk::Extent2D m_extent;

    uint32_t m_imageCount;
    vk::Format m_imageFormat;
    std::vector<vk::Image> m_images;
    std::vector<vk::ImageView> m_imageViews;

    vk::Format m_depthFormat;
    std::vector<vk::Image> m_depthImages;
    std::vector<vk::DeviceMemory> m_depthImageMemories;
    std::vector<vk::ImageView> m_depthImageViews;

    vk::RenderPass m_renderPass;
    std::vector<VkFramebuffer> m_framebuffers;

    std::vector<vk::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::Fence> m_inFlightFences;

    uint32_t m_frameIndex;
    uint32_t m_imageIndex;

    std::vector<vk::CommandBuffer> m_commandBuffers;

    bool m_isFrameStarted;
};
