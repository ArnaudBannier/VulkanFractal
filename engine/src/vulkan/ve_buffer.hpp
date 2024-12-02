/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "vulkan/ve_device.hpp"

class Buffer
{
public:
    Buffer(
        vk::Device device,
        const vk::PhysicalDeviceMemoryProperties &memoryProperties,
        uint32_t elementCount,
        vk::DeviceSize elementSize,
        vk::BufferUsageFlags usageFlags,
        vk::MemoryPropertyFlags memoryPropertyFlags,
        vk::DeviceSize minOffsetAlignment = 1);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    vk::Buffer getBuffer() const { return m_buffer; }
    void* getMappedMemory() const { return m_mapped; }
    uint32_t getElementCount() const { return m_elementCount; }
    vk::DeviceSize getElementSize() const { return m_elementSize; }
    vk::DeviceSize getAlignmentSize() const { return m_alignmentSize; }
    vk::BufferUsageFlags getUsageFlags() const { return m_usageFlags; }
    vk::MemoryPropertyFlags getMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
    vk::DeviceSize getBufferSize() const { return m_bufferSize; }


    void map(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
    void unmap();
    vk::DescriptorBufferInfo getDescriptorInfo(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

    void writeToBuffer(void *data, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
    void writeElementToBuffer(void *data, int index);

private:
    vk::Device m_device;
    vk::Buffer m_buffer;
    vk::DeviceMemory m_memory;
    void* m_mapped = nullptr;

    vk::DeviceSize m_bufferSize;
    uint32_t m_elementCount;
    vk::DeviceSize m_elementSize;
    vk::DeviceSize m_alignmentSize;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
};
