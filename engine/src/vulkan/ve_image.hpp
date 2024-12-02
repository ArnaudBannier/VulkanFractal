/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "ve_base.hpp"

class SamplerBuilder
{
public:
    SamplerBuilder();

    SamplerBuilder &setLinear();
    SamplerBuilder &setAddressMode(vk::SamplerAddressMode addressMode);
    vk::Sampler build(vk::Device device) const;

    SamplerBuilder &setFlags(vk::SamplerCreateFlags value) { m_createInfo.flags = value; return *this; }
    SamplerBuilder &setMagFilter(vk::Filter value) { m_createInfo.magFilter = value; return *this; }
    SamplerBuilder &setMinFilter(vk::Filter value) { m_createInfo.minFilter = value; return *this; }
    SamplerBuilder &setMipmapMode(vk::SamplerMipmapMode value) { m_createInfo.mipmapMode = value; return *this; }

    SamplerBuilder &setAddressModeU(vk::SamplerAddressMode value) { m_createInfo.addressModeU = value; return *this; }
    SamplerBuilder &setAddressModeV(vk::SamplerAddressMode value) { m_createInfo.addressModeV = value; return *this; }
    SamplerBuilder &setAddressModeW(vk::SamplerAddressMode value) { m_createInfo.addressModeW = value; return *this; }
    SamplerBuilder &setMipLodBias(float value) { m_createInfo.mipLodBias = value; return *this; }
    SamplerBuilder &setAnisotropyEnable(vk::Bool32 value) { m_createInfo.anisotropyEnable = value; return *this; }
    SamplerBuilder &setMaxAnisotropy(float value) { m_createInfo.maxAnisotropy = value; return *this; }
    SamplerBuilder &setCompareEnable(vk::Bool32 value) { m_createInfo.compareEnable = value; return *this; }
    SamplerBuilder &setCompareOp(vk::CompareOp value) { m_createInfo.compareOp = value; return *this; }
    SamplerBuilder &setMinLod(float value) { m_createInfo.minLod = value; return *this; }
    SamplerBuilder &setMaxLod(float value) { m_createInfo.maxLod = value; return *this; }
    SamplerBuilder &setBorderColor(vk::BorderColor value) { m_createInfo.borderColor = value; return *this; }
    SamplerBuilder &setUnnormalizedCoordinates(vk::Bool32 value) { m_createInfo.unnormalizedCoordinates = value; return *this; }

private:
    vk::SamplerCreateInfo m_createInfo;
};

class Image
{
public:
    Image(
        vk::Device device,
        const vk::PhysicalDeviceMemoryProperties &memoryProperties,
        const vk::ImageCreateInfo &imageCI,
        vk::ImageViewType viewType,
        vk::ImageAspectFlags aspectMask,
        bool makeHostImage);

    ~Image();

    void createTextureSampler(SamplerBuilder &samplerBuilder);

    void upload(
        const void *bytes,
        size_t byteCount,
        vk::CommandPool commandPool,
        vk::PhysicalDeviceMemoryProperties &memoryProperties,
        vk::Queue queue,
        vk::ImageLayout finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        bool uploadMipmaps = true);


    //Image(vk::Device device, const std::array<std::string, 6> &paths);
    //Image(vk::Device device, const std::string &path);

    vk::DescriptorImageInfo getDescriptorInfo() const;

    static vk::ImageCreateInfo defaultCreateInfo2D(uint32_t width, uint32_t height, vk::Format format);

    vk::Image getImage() const { return m_image; }
    vk::ImageView getView() const { return m_imageView; }
    vk::DeviceMemory getMemory() const { return m_imageMemory; }
    vk::ImageCreateInfo getCreateInfo() const { return m_imageCI; }

private:
    vk::Device m_device;

    vk::Image m_image;
    vk::DeviceMemory m_imageMemory;
    vk::ImageView m_imageView;
    vk::Sampler m_sampler;

    vk::ImageCreateInfo m_imageCI;
    vk::ImageAspectFlags m_aspectMask;
    vk::ImageLayout m_layout;

    void setImageLayout(
        vk::CommandBuffer commandbuffer,
        vk::ImageLayout newImageLayout,
        vk::ImageSubresourceRange subresourceRange);

    void generateMipmaps(
        vk::CommandBuffer commandBuffer,
        vk::ImageLayout finalLayout);

    void createCubeMap(const std::array<std::string, 6> &paths);
};

inline vk::DescriptorImageInfo Image::getDescriptorInfo() const
{
    vk::DescriptorImageInfo descriptor{ m_sampler, m_imageView, m_layout };
    return descriptor;
}