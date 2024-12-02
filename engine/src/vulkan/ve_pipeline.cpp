/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_pipeline.hpp"
#include "vulkan/ve_tools.hpp"

//==============================================================================
// Pipeline Layout

PipelineLayoutBuilder &PipelineLayoutBuilder::addDescriptorSetLayout(
    vk::DescriptorSetLayout descriptorSetLayout)
{
    m_descriptorSetLayouts.push_back(descriptorSetLayout);
    return *this;
}

PipelineLayoutBuilder &PipelineLayoutBuilder::addPushConstantRange(
    vk::PushConstantRange pushConstantRange)
{
    m_pushConstantRanges.push_back(pushConstantRange);
    return *this;
}


PipelineLayoutBuilder &PipelineLayoutBuilder::addPushConstantRange(
    vk::ShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size)
{
    vk::PushConstantRange pushConstantRange{ stageFlags, offset, size };
    m_pushConstantRanges.push_back(pushConstantRange);
    return *this;
}

vk::PipelineLayout PipelineLayoutBuilder::build(vk::Device &device)
{
    vk::PipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
    pipelineLayoutCI.pSetLayouts = m_descriptorSetLayouts.data();
    pipelineLayoutCI.pushConstantRangeCount = static_cast<uint32_t>(m_pushConstantRanges.size());
    pipelineLayoutCI.pPushConstantRanges = m_pushConstantRanges.data();

    return device.createPipelineLayout(pipelineLayoutCI);
}


//==============================================================================
// Pipeline

PipelineBuilder::PipelineBuilder(
    vk::PipelineLayout pipelineLayout,
    vk::RenderPass renderPass,
    vk::PipelineCache pipelineCache)
    : m_pipelineLayout{ pipelineLayout }
    , m_renderPass{ renderPass }
    , m_polygonMode{ VK_POLYGON_MODE_FILL }
    , m_pipelineCache{ pipelineCache }
    , m_cullMode{ vk::CullModeFlagBits::eNone }
    , m_frontFace{ vk::FrontFace::eCounterClockwise }
    , m_rasterizationSamples{ vk::SampleCountFlagBits::e1 }
    , m_primitiveTopology{ vk::PrimitiveTopology::eTriangleList }
    , m_patchControlPoints{ 0 }
{
}

PipelineBuilder &PipelineBuilder::addVertexBindingDescription(
    vk::VertexInputBindingDescription &vertexBindingDescription)
{
    m_vertexBindings.push_back(vertexBindingDescription);
    return *this;
}

PipelineBuilder &PipelineBuilder::addVertexBindingDescription(
    uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate)
{
    vk::VertexInputBindingDescription vertexBindingDescription(binding, stride, inputRate);
    return addVertexBindingDescription(vertexBindingDescription);
}

PipelineBuilder &PipelineBuilder::addVertexAttributeDescription(
    vk::VertexInputAttributeDescription &vertexAttributeDescription)
{
    m_vertexAttributes.push_back(vertexAttributeDescription);
    return *this;
}

PipelineBuilder &PipelineBuilder::addVertexAttributeDescription(
    uint32_t location, uint32_t binding, vk::Format format, uint32_t offset)
{
    vk::VertexInputAttributeDescription vertexAttributeDescription(location, binding, format, offset);
    return addVertexAttributeDescription(vertexAttributeDescription);
}

PipelineBuilder &PipelineBuilder::addShaderStage(
    vk::PipelineShaderStageCreateInfo &shaderStage)
{
    m_shaderStages.push_back(shaderStage);
    return *this;
}

PipelineBuilder &PipelineBuilder::setCullMode(vk::CullModeFlags cullMode)
{
    m_cullMode = cullMode;
    return *this;
}

PipelineBuilder &PipelineBuilder::setFrontFace(vk::FrontFace frontFace)
{
    m_frontFace = frontFace;
    return *this;
}

PipelineBuilder &PipelineBuilder::setPolygonMode(vk::PolygonMode polygonMode)
{
    m_polygonMode = polygonMode;
    return *this;
}

PipelineBuilder &PipelineBuilder::setPipelineCache(vk::PipelineCache pipelineCache)
{
    m_pipelineCache = pipelineCache;
    return *this;
}

PipelineBuilder &PipelineBuilder::setRasterizationSamples(
    vk::SampleCountFlagBits rasterizationSamples)
{
    m_rasterizationSamples = rasterizationSamples;
    return *this;
}

PipelineBuilder &PipelineBuilder::setPrimitiveTopology(vk::PrimitiveTopology primitiveTopology)
{
    m_primitiveTopology = primitiveTopology;
    return *this;
}

PipelineBuilder &PipelineBuilder::setTessellationPatchControlPoints(uint32_t patchControlPoints)
{
    m_primitiveTopology = vk::PrimitiveTopology::ePatchList;
    m_patchControlPoints = patchControlPoints;
    return *this;
}

vk::Pipeline PipelineBuilder::build(vk::Device &device) const
{
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
    inputAssemblyStateCI.topology = m_primitiveTopology;
    inputAssemblyStateCI.primitiveRestartEnable = vk::False;

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCI{};
    rasterizationStateCI.polygonMode = m_polygonMode;
    rasterizationStateCI.cullMode = m_cullMode;
    rasterizationStateCI.frontFace = m_frontFace;
    rasterizationStateCI.lineWidth = 1.0f;
    rasterizationStateCI.depthBiasEnable = vk::False;
    rasterizationStateCI.depthBiasConstantFactor = 0.0f;
    rasterizationStateCI.depthBiasClamp = 0.0f;
    rasterizationStateCI.depthBiasSlopeFactor = 0.0f;

    vk::PipelineColorBlendAttachmentState blendAttachmentState{};
    blendAttachmentState.colorWriteMask =
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
    blendAttachmentState.blendEnable = VK_FALSE;
    blendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne;
    blendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero;
    blendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    blendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    blendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    blendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCI{};
    colorBlendStateCI.attachmentCount = 1;
    colorBlendStateCI.pAttachments = &blendAttachmentState;
    colorBlendStateCI.logicOpEnable = vk::False;
    colorBlendStateCI.logicOp = vk::LogicOp::eCopy;

    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCI{};
    depthStencilStateCI.depthTestEnable = vk::True;
    depthStencilStateCI.depthWriteEnable = vk::True;
    depthStencilStateCI.depthCompareOp = vk::CompareOp::eLessOrEqual;
    depthStencilStateCI.depthBoundsTestEnable = vk::False;
    depthStencilStateCI.minDepthBounds = 0.0f;
    depthStencilStateCI.maxDepthBounds = 1.0f;
    depthStencilStateCI.stencilTestEnable = vk::False;
    depthStencilStateCI.front = depthStencilStateCI.back;
    depthStencilStateCI.back.compareOp = vk::CompareOp::eAlways;

    vk::PipelineViewportStateCreateInfo viewportStateCI{};
    viewportStateCI.viewportCount = 1;
    viewportStateCI.scissorCount = 1;

    vk::PipelineMultisampleStateCreateInfo multisampleStateCI{};
    multisampleStateCI.sampleShadingEnable = vk::False;
    multisampleStateCI.rasterizationSamples = m_rasterizationSamples;
    multisampleStateCI.minSampleShading = 1.0f;
    multisampleStateCI.pSampleMask = nullptr;
    multisampleStateCI.alphaToCoverageEnable = vk::False;
    multisampleStateCI.alphaToOneEnable = vk::False;

    std::vector<vk::DynamicState> dynamicStateEnables = {
        vk::DynamicState::eViewport, vk::DynamicState::eScissor
    };
    vk::PipelineDynamicStateCreateInfo dynamicStateCI{};
    dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
    dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCI{};
    vertexInputStateCI.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertexBindings.size());
    vertexInputStateCI.pVertexBindingDescriptions = m_vertexBindings.data();
    vertexInputStateCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertexAttributes.size());
    vertexInputStateCI.pVertexAttributeDescriptions = m_vertexAttributes.data();

    vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCI{};
    pipelineTessellationStateCI.patchControlPoints = m_patchControlPoints;

    // Pipeline
    vk::GraphicsPipelineCreateInfo pipelineCI{};
    pipelineCI.layout = m_pipelineLayout;
    pipelineCI.renderPass = m_renderPass;
    pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
    pipelineCI.pVertexInputState = &vertexInputStateCI;
    pipelineCI.pRasterizationState = &rasterizationStateCI;
    pipelineCI.pColorBlendState = &colorBlendStateCI;
    pipelineCI.pMultisampleState = &multisampleStateCI;
    pipelineCI.pViewportState = &viewportStateCI;
    pipelineCI.pDepthStencilState = &depthStencilStateCI;
    pipelineCI.pDynamicState = &dynamicStateCI;
    pipelineCI.stageCount = static_cast<uint32_t>(m_shaderStages.size());
    pipelineCI.pStages = m_shaderStages.data();

    if (m_patchControlPoints > 0)
        pipelineCI.pTessellationState = &pipelineTessellationStateCI;

    auto [result, pipeline] = device.createGraphicsPipeline(m_pipelineCache, pipelineCI);
    return pipeline;
}
