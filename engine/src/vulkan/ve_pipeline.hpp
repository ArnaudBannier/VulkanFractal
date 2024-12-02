/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "vulkan/ve_device.hpp"

//==============================================================================
// Pipeline Layout

class PipelineLayoutBuilder
{
public:
    PipelineLayoutBuilder() {}

    PipelineLayoutBuilder &addDescriptorSetLayout(
        vk::DescriptorSetLayout descriptorSetLayout);

    PipelineLayoutBuilder &addPushConstantRange(
        vk::PushConstantRange pushConstantRange);

    PipelineLayoutBuilder &addPushConstantRange(
        vk::ShaderStageFlags stageFlags,
        uint32_t offset,
        uint32_t size);

    vk::PipelineLayout build(vk::Device &device);

private:
    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts{};
    std::vector<vk::PushConstantRange> m_pushConstantRanges{};
};

//==============================================================================
// Pipeline

class PipelineBuilder
{
public:
    PipelineBuilder(
        vk::PipelineLayout pipelineLayout,
        vk::RenderPass renderPass,
        vk::PipelineCache pipelineCache = VK_NULL_HANDLE);

    PipelineBuilder &addVertexBindingDescription(
        vk::VertexInputBindingDescription &vertexBindingDescription);
    PipelineBuilder &addVertexBindingDescription(
        uint32_t binding,
        uint32_t stride,
        vk::VertexInputRate inputRate);

    PipelineBuilder &addVertexAttributeDescription(
        vk::VertexInputAttributeDescription &vertexAttributeDescription);
    PipelineBuilder &addVertexAttributeDescription(
        uint32_t location,
        uint32_t binding,
        vk::Format format,
        uint32_t offset);

    PipelineBuilder &addShaderStage(
        vk::PipelineShaderStageCreateInfo &shaderStage);

    PipelineBuilder &setCullMode(vk::CullModeFlags cullMode);
    PipelineBuilder &setFrontFace(vk::FrontFace frontFace);

    PipelineBuilder &setPolygonMode(vk::PolygonMode polygonMode);
    PipelineBuilder &setPipelineCache(vk::PipelineCache pipelineCache);
    PipelineBuilder &setRasterizationSamples(vk::SampleCountFlagBits rasterizationSamples);
    PipelineBuilder &setPrimitiveTopology(vk::PrimitiveTopology primitiveTopology);
    PipelineBuilder &setTessellationPatchControlPoints(uint32_t patchControlPoints);

    vk::Pipeline build(vk::Device &device) const;

private:
    vk::PipelineLayout m_pipelineLayout;
    vk::PipelineCache m_pipelineCache;
    vk::RenderPass m_renderPass;

    std::vector<vk::VertexInputBindingDescription> m_vertexBindings{};
    std::vector<vk::VertexInputAttributeDescription> m_vertexAttributes{};
    std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages{};

    vk::PolygonMode m_polygonMode;
    vk::CullModeFlags m_cullMode;
    vk::FrontFace m_frontFace;
    vk::SampleCountFlagBits m_rasterizationSamples;
    vk::PrimitiveTopology m_primitiveTopology;
    uint32_t m_patchControlPoints;
};
