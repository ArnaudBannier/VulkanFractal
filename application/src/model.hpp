/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve.hpp"

class FullscreenModel
{
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 texCoord;
    };

    FullscreenModel(VulkanBase &base);
    ~FullscreenModel();

    FullscreenModel(const FullscreenModel &) = delete;
    FullscreenModel &operator=(const FullscreenModel &) = delete;

    void bind(vk::CommandBuffer commandBuffer);
    void draw(vk::CommandBuffer commandBuffer);

private:
    VulkanBase &m_base;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;

    void createVertexBuffer(const std::vector<Vertex> &vertices);
    void createIndexBuffer(const std::vector<uint32_t> &indices);
};
