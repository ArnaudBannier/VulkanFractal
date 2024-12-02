/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_image.hpp"
#include "vulkan/ve_buffer.hpp"
#include "vulkan/ve_tools.hpp"

//void Image::createCubeMap(const std::array<std::string, 6> &paths)
//{
////    int texWidths[6] = { 0 };
////    int texHeights[6] = { 0 };
////    int texChannels[6] = { 0 };
////
////    stbi_uc *pixels[6] = { nullptr };
////    for (int i = 0; i < 6; i++)
////    {
////        pixels[i] = stbi_load(
////            paths[i].c_str(),
////            &texWidths[i], &texHeights[i], &texChannels[i], STBI_rgb_alpha);
////
////        if (!pixels[i])
////        {
////            throw std::runtime_error("failed to load texture image!");
////        }
////
////        assert(texWidths[i] == texWidths[0]);
////        assert(texHeights[i] == texHeights[0]);
////        assert(texChannels[i] == texChannels[0]);
////    }
////
////    m_width = static_cast<uint32_t>(texWidths[0]);
////    m_height = static_cast<uint32_t>(texHeights[0]);
////    m_mipLevels = 1;// static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
////
////    const VkDeviceSize layerSize = m_width * m_height * 4;
////    const VkDeviceSize imageSize = layerSize * 6;
////
////    VkBuffer stagingBuffer;
////    VkDeviceMemory stagingBufferMemory;
////    m_device.createBuffer(
////        imageSize,
////        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
////        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
////        stagingBuffer,
////        stagingBufferMemory);
////
////    VkDevice device = m_device.getLogicalDevice();
////    uint8_t *data;
////    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, (void **)&data);
////    for (uint32_t face = 0; face < 6; face++)
////    {
////        memcpy(data + face * layerSize, pixels[face], static_cast<size_t>(layerSize));
////    }
////    vkUnmapMemory(device, stagingBufferMemory);
////
////
////    m_device.createCubeImage(
////        m_width, m_height, m_mipLevels,
////        m_format,
////        VK_IMAGE_TILING_OPTIMAL,
////        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
////        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
////        m_image,
////        m_imageMemory);
////
////    ///////////////////////////////////
////    
////    // Setup buffer copy regions for each face (TODO : including all of its mip levels)
////    std::vector<VkBufferImageCopy> bufferCopyRegions;
////
////    for (uint32_t face = 0; face < 6; face++)
////    {
////        VkDeviceSize offset = face * layerSize;
////
////        VkBufferImageCopy bufferCopyRegion = {};
////        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
////        bufferCopyRegion.imageSubresource.mipLevel = 0;
////        bufferCopyRegion.imageSubresource.baseArrayLayer = face;
////        bufferCopyRegion.imageSubresource.layerCount = 1;
////        bufferCopyRegion.imageExtent.width = m_width;
////        bufferCopyRegion.imageExtent.height = m_height;
////        bufferCopyRegion.imageExtent.depth = 1;
////        bufferCopyRegion.bufferOffset = offset;
////
////        bufferCopyRegions.push_back(bufferCopyRegion);
////    }
////
////    VkCommandBuffer copyCmd = m_device.beginSingleTimeCommands();
////
////    // Image barrier for optimal image (target)
////    // Set initial layout for all array layers (faces) of the optimal (target) tiled texture
////    VkImageSubresourceRange subresourceRange = {};
////    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
////    subresourceRange.baseMipLevel = 0;
////    subresourceRange.levelCount = 1;
////    subresourceRange.layerCount = 6;
////
////    setImageLayout(
////        copyCmd,
////        m_image,
////        VK_IMAGE_LAYOUT_UNDEFINED,
////        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
////        subresourceRange);
////
////    // Copy the cube map faces from the staging buffer to the optimal tiled image
////    vkCmdCopyBufferToImage(
////        copyCmd,
////        stagingBuffer,
////        m_image,
////        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
////        static_cast<uint32_t>(bufferCopyRegions.size()),
////        bufferCopyRegions.data());
////
////    // Change texture image layout to shader read after all faces have been copied
////    setImageLayout(
////        copyCmd,
////        m_image,
////        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
////        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
////        subresourceRange);
////
////    m_device.endSingleTimeCommands(copyCmd);
////
////    // Create sampler
////    VkPhysicalDeviceProperties properties{};
////    vkGetPhysicalDeviceProperties(m_device.getPhysicalDevice(), &properties);
////
////    VkSamplerCreateInfo samplerCreateInfo {};
////    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
////    samplerCreateInfo.maxAnisotropy = 1.0f;
////    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
////    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
////    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
////    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
////    samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
////    samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
////    samplerCreateInfo.mipLodBias = 0.0f;
////    samplerCreateInfo.anisotropyEnable = VK_TRUE;
////    samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
////    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
////    samplerCreateInfo.minLod = 0.0f;
////    samplerCreateInfo.maxLod = (float)m_mipLevels;
////    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
////
////    VK_CHECK_RESULT(vkCreateSampler(m_device.getLogicalDevice(), &samplerCreateInfo, nullptr, &m_sampler));
////
////    // Create image view
////    VkImageViewCreateInfo viewCreateInfo{};
////    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
////    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
////    viewCreateInfo.format = m_format;
////    viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
////    viewCreateInfo.subresourceRange.layerCount = 6;
////    viewCreateInfo.subresourceRange.levelCount = m_mipLevels;
////    viewCreateInfo.image = m_image;
////    VK_CHECK_RESULT(vkCreateImageView(m_device.getLogicalDevice(), &viewCreateInfo, nullptr, &m_imageView));
////
////    // Clean up staging resources
////    for (int face = 0; face < 6; face++)
////    {
////        stbi_image_free(pixels[face]);
////    }
////    vkDestroyBuffer(device, stagingBuffer, nullptr);
////    vkFreeMemory(device, stagingBufferMemory, nullptr);
////
////    // Update descriptor image info member that can be used for setting up descriptor sets
////    updateDescriptor();
//}

SamplerBuilder::SamplerBuilder()
    : m_createInfo{}
{
    m_createInfo.magFilter = vk::Filter::eNearest;
    m_createInfo.minFilter = vk::Filter::eNearest;
    m_createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    m_createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    m_createInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    m_createInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    m_createInfo.mipLodBias = 0.0f;
    m_createInfo.anisotropyEnable = vk::False;
    m_createInfo.maxAnisotropy = 0.0f;
    m_createInfo.compareEnable = vk::False;
    m_createInfo.compareOp = vk::CompareOp::eNever;
    m_createInfo.minLod = 0;
    m_createInfo.maxLod = 0;
    m_createInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    m_createInfo.unnormalizedCoordinates = vk::False;
}

SamplerBuilder &SamplerBuilder::setLinear()
{
    m_createInfo.magFilter = vk::Filter::eLinear;
    m_createInfo.minFilter = vk::Filter::eLinear;
    m_createInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    return *this;
}

SamplerBuilder &SamplerBuilder::setAddressMode(vk::SamplerAddressMode addressMode)
{
    m_createInfo.addressModeU = addressMode;
    m_createInfo.addressModeV = addressMode;
    m_createInfo.addressModeW = addressMode;
    return *this;
}

vk::Sampler SamplerBuilder::build(vk::Device device) const
{
    return device.createSampler(m_createInfo);
}

void Image::setImageLayout(
    vk::CommandBuffer commandbuffer,
    vk::ImageLayout newImageLayout,
    vk::ImageSubresourceRange subresourceRange)
{
    if (m_layout == newImageLayout) return;

    // Create an image barrier object
    vk::ImageMemoryBarrier imageMemoryBarrier {};
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageMemoryBarrier.oldLayout = m_layout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = m_image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vk::PipelineStageFlags srcStageMask{};
    vk::PipelineStageFlags dstStageMask{};
    vk::AccessFlags srcAccessMask{};
    vk::AccessFlags dstAccessMask{};

    switch (m_layout)
    {
    case vk::ImageLayout::eGeneral:
        srcAccessMask = vk::AccessFlagBits::eNone;
        srcStageMask = vk::PipelineStageFlagBits::eHost;
        break;

    case vk::ImageLayout::eUndefined:
        srcAccessMask = vk::AccessFlagBits::eNone;
        srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
        break;

    case vk::ImageLayout::ePreinitialized:
        srcAccessMask = vk::AccessFlagBits::eHostWrite;
        srcStageMask = vk::PipelineStageFlagBits::eHost;
        break;

    case vk::ImageLayout::eColorAttachmentOptimal:
        srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        break;

    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthAttachmentOptimal:
        srcAccessMask =
            vk::AccessFlagBits::eDepthStencilAttachmentRead |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        break;

    case vk::ImageLayout::eTransferSrcOptimal:
        srcAccessMask = vk::AccessFlagBits::eTransferRead;
        srcStageMask = vk::PipelineStageFlagBits::eTransfer;
        break;

    case vk::ImageLayout::eTransferDstOptimal:
        srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        srcStageMask = vk::PipelineStageFlagBits::eTransfer;
        break;

    case vk::ImageLayout::eShaderReadOnlyOptimal:
        srcAccessMask = vk::AccessFlagBits::eShaderRead;
        srcStageMask = vk::PipelineStageFlagBits::eVertexShader;
        break;

    case vk::ImageLayout::ePresentSrcKHR:
        srcAccessMask = vk::AccessFlagBits::eMemoryRead;
        srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
        break;

    default:
        // Other source layouts aren't handled (yet)
        assert(false);
        break;
    }

    switch (newImageLayout)
    {

    case vk::ImageLayout::eGeneral:
        dstAccessMask = vk::AccessFlagBits::eNone;
        dstStageMask = vk::PipelineStageFlagBits::eHost;
        break;

    case vk::ImageLayout::ePresentSrcKHR:
        dstAccessMask = vk::AccessFlagBits::eNone;
        dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
        break;

    case vk::ImageLayout::eTransferSrcOptimal:
        dstAccessMask = vk::AccessFlagBits::eTransferRead;
        dstStageMask = vk::PipelineStageFlagBits::eTransfer;
        break;

    case vk::ImageLayout::eTransferDstOptimal:
        dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        dstStageMask = vk::PipelineStageFlagBits::eTransfer;
        break;

    case vk::ImageLayout::eColorAttachmentOptimal:
        dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        break;

    case vk::ImageLayout::eDepthAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        dstAccessMask =
            vk::AccessFlagBits::eDepthStencilAttachmentRead |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        break;

    case vk::ImageLayout::eShaderReadOnlyOptimal:
        if (srcAccessMask == vk::AccessFlagBits::eNone)
        {
            srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
        }
        dstAccessMask = vk::AccessFlagBits::eShaderRead;
        dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
        break;

    default:
        // Other source layouts aren't handled (yet)
        assert(false);
        break;
    }
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;

    vk::DependencyFlags dependencyFlags{};
    auto memoryBarriers = nullptr;
    auto bufferMemoryBarriers = nullptr;
    
    // Put barrier inside setup command buffer
    commandbuffer.pipelineBarrier(
        srcStageMask,
        dstStageMask,
        dependencyFlags,
        memoryBarriers,
        bufferMemoryBarriers,
        imageMemoryBarrier);

    m_layout = newImageLayout;
}

Image::Image(
    vk::Device device,
    const vk::PhysicalDeviceMemoryProperties &memoryProperties,
    const vk::ImageCreateInfo &imageCI,
    vk::ImageViewType viewType,
    vk::ImageAspectFlags aspectMask,
    bool makeHostImage
)
    : m_device{ device }
    , m_imageCI{ imageCI }
    , m_aspectMask{ aspectMask }
    , m_layout { imageCI.initialLayout }
{
    // Create the image
    m_image = m_device.createImage(imageCI);

    // Create memory and bind it
    vk::MemoryRequirements memoryRequirements =
        m_device.getImageMemoryRequirements(m_image);

    vk::MemoryPropertyFlags search = vk::MemoryPropertyFlagBits::eDeviceLocal;
    if (makeHostImage)
    {
        search =
            vk::MemoryPropertyFlagBits::eHostCoherent |
            vk::MemoryPropertyFlagBits::eHostVisible;
    }

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = tools::findMemoryTypeIndex(
        memoryProperties,
        memoryRequirements.memoryTypeBits,
        search);

    m_imageMemory = m_device.allocateMemory(allocInfo);
    m_device.bindImageMemory(m_image, m_imageMemory, 0);

    if (makeHostImage == false)
    {
        // Create image view
        vk::ImageViewCreateInfo imageViewCI{};
        imageViewCI.image = m_image;
        imageViewCI.viewType = viewType;
        imageViewCI.format = imageCI.format;
        imageViewCI.subresourceRange.aspectMask = aspectMask;
        imageViewCI.subresourceRange.baseMipLevel = 0;
        imageViewCI.subresourceRange.levelCount = imageCI.mipLevels;
        imageViewCI.subresourceRange.baseArrayLayer = 0;
        imageViewCI.subresourceRange.layerCount = imageCI.arrayLayers;
        imageViewCI.components = {
            vk::ComponentSwizzle::eR,
            vk::ComponentSwizzle::eG,
            vk::ComponentSwizzle::eB,
            vk::ComponentSwizzle::eA
        };

        m_imageView = m_device.createImageView(imageViewCI);
    }
}

void Image::createTextureSampler(SamplerBuilder &samplerBuilder)
{
    m_sampler = samplerBuilder.build(m_device);
}

void Image::upload(
    const void *bytes,
    size_t byteCount,
    vk::CommandPool commandPool,
    vk::PhysicalDeviceMemoryProperties &memoryProperties,
    vk::Queue queue,
    vk::ImageLayout finalLayout,
    bool uploadMipmaps)
{
    Buffer stagingBuffer{
        m_device,
        memoryProperties,
        1,
        byteCount,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)bytes);
    stagingBuffer.unmap();

    vk::CommandBuffer commandBuffer = tools::beginSingleTimeCommands(m_device, commandPool);

    vk::ImageSubresourceRange imageSubresourceRange{};
    imageSubresourceRange.aspectMask = m_aspectMask;
    imageSubresourceRange.baseMipLevel = 0;
    imageSubresourceRange.levelCount = m_imageCI.mipLevels;
    imageSubresourceRange.baseArrayLayer = 0;
    imageSubresourceRange.layerCount = m_imageCI.arrayLayers;

    setImageLayout(
        commandBuffer, vk::ImageLayout::eTransferDstOptimal,
        imageSubresourceRange
    );

    vk::BufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = m_aspectMask;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = m_imageCI.arrayLayers;
    region.setImageOffset({ 0, 0, 0 });
    region.imageExtent = m_imageCI.extent;

    commandBuffer.copyBufferToImage(
        stagingBuffer.getBuffer(),
        m_image,
        vk::ImageLayout::eTransferDstOptimal,
        1, &region);

    if (uploadMipmaps)
    {
        generateMipmaps(commandBuffer, finalLayout);
    }
    else
    {
        setImageLayout(
            commandBuffer, finalLayout,
            imageSubresourceRange
        );
    }

    tools::endSingleTimeCommands(m_device, queue, commandPool, commandBuffer);
}

void Image::generateMipmaps(
    vk::CommandBuffer commandBuffer,
    vk::ImageLayout finalLayout)
{
    vk::ImageMemoryBarrier barrier{};
    barrier.image = m_image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = m_aspectMask;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t prevWidth = m_imageCI.extent.width;
    int32_t prevHeight = m_imageCI.extent.height;
    vk::DependencyFlags dependencyFlags{};
    auto memoryBarriers = nullptr;
    auto bufferMemoryBarriers = nullptr;

    for (uint32_t i = 1; i < m_imageCI.mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            dependencyFlags,
            memoryBarriers,
            bufferMemoryBarriers,
            barrier);

        int32_t currWidth = prevWidth > 1 ? prevWidth >> 1 : 1;
        int32_t currHeight = prevHeight > 1 ? prevHeight >> 1 : 1;

        vk::ImageBlit blit{};
        blit.setSrcOffsets({
            vk::Offset3D{ 0, 0, 0 },
            vk::Offset3D{ prevWidth, prevHeight, 1 } });
        blit.srcSubresource.aspectMask = m_aspectMask;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.setDstOffsets({
            vk::Offset3D{ 0, 0, 0 },
            vk::Offset3D{ currWidth, currHeight, 1 } });
        blit.dstSubresource.aspectMask = m_aspectMask;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        commandBuffer.blitImage(
            m_image, vk::ImageLayout::eTransferSrcOptimal,
            m_image, vk::ImageLayout::eTransferDstOptimal,
            1, &blit,
            vk::Filter::eLinear);

        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout = finalLayout;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            dependencyFlags,
            memoryBarriers,
            bufferMemoryBarriers,
            barrier);

        prevWidth = currWidth;
        prevHeight = currHeight;
    }

    barrier.subresourceRange.baseMipLevel = m_imageCI.mipLevels - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = finalLayout;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        dependencyFlags,
        memoryBarriers,
        bufferMemoryBarriers,
        barrier);

    m_layout = finalLayout;
}

Image::~Image()
{
    if (m_sampler) m_device.destroySampler(m_sampler);
    if (m_imageView) m_device.destroyImageView(m_imageView);
    m_device.freeMemory(m_imageMemory);
    m_device.destroyImage(m_image);
}

vk::ImageCreateInfo Image::defaultCreateInfo2D(uint32_t width, uint32_t height, vk::Format format)
{
    vk::ImageCreateInfo imageCI{};
    imageCI.imageType = vk::ImageType::e2D;
    imageCI.extent.width = width;
    imageCI.extent.height = height;
    imageCI.extent.depth = 1;
    imageCI.mipLevels = 1;
    imageCI.arrayLayers = 1;
    imageCI.initialLayout = vk::ImageLayout::eUndefined;
    imageCI.samples = vk::SampleCountFlagBits::e1;
    imageCI.sharingMode = vk::SharingMode::eExclusive;
    imageCI.format = format;
    imageCI.tiling = vk::ImageTiling::eOptimal;
    imageCI.usage =
        vk::ImageUsageFlagBits::eTransferSrc |
        vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled;
    return imageCI;
}
