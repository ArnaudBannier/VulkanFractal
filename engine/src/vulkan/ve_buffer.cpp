/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_buffer.hpp"
#include "vulkan/ve_tools.hpp"

#include <cassert>
#include <cstring>
#include <iostream>

Buffer::Buffer(
    vk::Device device,
    const vk::PhysicalDeviceMemoryProperties &memoryProperties,
    uint32_t elementCount,
    vk::DeviceSize elementSize,
    vk::BufferUsageFlags usageFlags,
    vk::MemoryPropertyFlags memoryPropertyFlags,
    vk::DeviceSize minOffsetAlignment
)
    : m_device{ device }
    , m_elementSize{ elementSize }
    , m_elementCount{ elementCount }
    , m_usageFlags{ usageFlags }
    , m_memoryPropertyFlags{ memoryPropertyFlags }
    , m_mapped{ nullptr }
{
    m_alignmentSize = tools::alignedVkSize(elementSize, minOffsetAlignment);
    m_bufferSize = m_alignmentSize * elementCount;
    assert(m_bufferSize > 0 && "The buffer size must be non zero");

    vk::BufferCreateInfo bufferCI{};
    bufferCI.size = m_bufferSize;
    bufferCI.usage = usageFlags;
    bufferCI.sharingMode = vk::SharingMode::eExclusive;

    m_buffer = m_device.createBuffer(bufferCI);

    vk::MemoryRequirements memoryRequirements = m_device.getBufferMemoryRequirements(m_buffer);
    vk::MemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = tools::findMemoryTypeIndex(
        memoryProperties, memoryRequirements.memoryTypeBits, memoryPropertyFlags);

    m_memory = m_device.allocateMemory(memoryAllocateInfo);
    m_device.bindBufferMemory(m_buffer, m_memory, 0);
}

Buffer::~Buffer()
{
    unmap();
    m_device.freeMemory(m_memory);
    m_device.destroyBuffer(m_buffer);
}

void Buffer::map(vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(m_buffer && m_memory && "Called map on buffer before create");

    m_mapped = m_device.mapMemory(m_memory, offset, size);
}

void Buffer::unmap()
{
    if (m_mapped)
    {
        m_device.unmapMemory(m_memory);
        m_mapped = nullptr;
    }
}

vk::DescriptorBufferInfo Buffer::getDescriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo{m_buffer, offset, size};
}

void Buffer::writeToBuffer(void *data, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(m_mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        memcpy(m_mapped, data, m_bufferSize);
    }
    else
    {
        char *memOffset = (char *)m_mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

void Buffer::writeElementToBuffer(void *data, int index)
{
    assert(m_mapped && "Cannot copy to unmapped buffer");

    char *memOffset = (char *)m_mapped;
    memOffset += index * m_alignmentSize;
    memcpy(memOffset, data, m_elementSize);
}
