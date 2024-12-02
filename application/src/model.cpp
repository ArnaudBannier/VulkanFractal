/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "model.hpp"

FullscreenModel::FullscreenModel(VulkanBase &base)
    : m_base{ base }
    , m_vertexBuffer{ nullptr }
    , m_indexBuffer{ nullptr }
{
    // Coordinates
    // 0 - 3
    // | \ |
    // 1 - 2

    // Vulkan NDC: from [-1,-1] top left corner to [1,1] bottom right corner.

    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{
        0, 1, 2, // Bottom left
        0, 2, 3  // Top right
    };
    Vertex vertex{};
    vertex.pos = { -1.f, -1.f, 0.f }; vertex.texCoord = { 0.f, 0.f }; vertices.push_back(vertex);
    vertex.pos = { -1.f, +1.f, 0.f }; vertex.texCoord = { 0.f, 1.f }; vertices.push_back(vertex);
    vertex.pos = { +1.f, +1.f, 0.f }; vertex.texCoord = { 1.f, 1.f }; vertices.push_back(vertex);
    vertex.pos = { +1.f, -1.f, 0.f }; vertex.texCoord = { 1.f, 0.f }; vertices.push_back(vertex);

    createVertexBuffer(vertices);
    createIndexBuffer(indices);
}

FullscreenModel::~FullscreenModel()
{
}

void FullscreenModel::bind(vk::CommandBuffer commandBuffer)
{
    commandBuffer.bindVertexBuffers(0, { m_vertexBuffer->getBuffer() }, { 0 });
    commandBuffer.bindIndexBuffer(m_indexBuffer->getBuffer(), 0, vk::IndexType::eUint32);
}

void FullscreenModel::draw(vk::CommandBuffer commandBuffer)
{
    commandBuffer.drawIndexed(
        static_cast<uint32_t>(m_indexBuffer->getElementCount()),
        1, 0, 0, 0);
}

void FullscreenModel::createVertexBuffer(const std::vector<Vertex> &vertices)
{
    uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");

    uint32_t vertexSize = sizeof(vertices[0]);

    vk::PhysicalDeviceMemoryProperties memoryProperties = m_base.getMemoryProperties();

    Buffer stagingBuffer{
        m_base.getDevice(),
        memoryProperties,
        vertexCount,
        vertexSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    m_vertexBuffer = std::make_unique<Buffer>(
        m_base.getDevice(),
        memoryProperties,
        vertexCount,
        vertexSize,
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    vk::CommandBuffer commandBuffer = tools::beginSingleTimeCommands(m_base);

    vk::BufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = stagingBuffer.getBufferSize();
    commandBuffer.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), 1, &copyRegion);

    tools::endSingleTimeCommands(m_base, commandBuffer);
}

void FullscreenModel::createIndexBuffer(const std::vector<uint32_t> &indices)
{
    uint32_t indexCount = static_cast<uint32_t>(indices.size());
    if (indexCount == 0) return;

    uint32_t indexSize = sizeof(indices[0]);

    Buffer stagingBuffer{
        m_base.getDevice(),
        m_base.getMemoryProperties(),
        indexCount,
        indexSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    m_indexBuffer = std::make_unique<Buffer>(
        m_base.getDevice(),
        m_base.getMemoryProperties(),
        indexCount,
        indexSize,
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::CommandBuffer commandBuffer = tools::beginSingleTimeCommands(m_base);

    vk::BufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = stagingBuffer.getBufferSize();
    commandBuffer.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), 1, &copyRegion);

    tools::endSingleTimeCommands(m_base, commandBuffer);
}
