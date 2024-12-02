/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_descriptor.hpp"
#include "vulkan/ve_tools.hpp"

//==============================================================================
// Descriptor Pool

DescriptorPoolBuilder::DescriptorPoolBuilder()
    : m_maxSets{ 256 }, m_poolSizes{}, m_poolFlags{}
{
}

DescriptorPoolBuilder &DescriptorPoolBuilder::addPoolSize(vk::DescriptorType descriptorType, uint32_t count)
{
    vk::DescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.descriptorCount = count;
    descriptorPoolSize.type = descriptorType;

    m_poolSizes.push_back(descriptorPoolSize);
    return *this;
}

DescriptorPoolBuilder &DescriptorPoolBuilder::setPoolFlags(vk::DescriptorPoolCreateFlags flags)
{
    m_poolFlags = flags;
    return *this;
}

DescriptorPoolBuilder &DescriptorPoolBuilder::setMaxSets(uint32_t count)
{
    m_maxSets = count;
    return *this;
}

vk::DescriptorPool DescriptorPoolBuilder::build(vk::Device device)
{
    vk::DescriptorPoolCreateInfo descriptorPoolCI{};
    descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
    descriptorPoolCI.pPoolSizes = m_poolSizes.data();
    descriptorPoolCI.maxSets = m_maxSets;
    descriptorPoolCI.flags = m_poolFlags;

    return device.createDescriptorPool(descriptorPoolCI);
}

//==============================================================================
// Descriptor Set Layout

DescriptorSetLayoutBuilder &DescriptorSetLayoutBuilder::addBinding(
    uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count)
{
    assert(m_bindings.count(binding) == 0 && "Binding already in use");

    vk::DescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    layoutBinding.pImmutableSamplers = nullptr;

    m_bindings[binding] = layoutBinding;
    return *this;
}

vk::DescriptorSetLayout DescriptorSetLayoutBuilder::build(vk::Device device)
{
    std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto &kv : m_bindings)
    {
        setLayoutBindings.push_back(kv.second);
    }

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCI{};
    descriptorSetLayoutCI.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutCI.pBindings = setLayoutBindings.data();

    return device.createDescriptorSetLayout(descriptorSetLayoutCI);
}

//==============================================================================
// Descriptor Set

DescriptorSetBuilder &DescriptorSetBuilder::addLayout(vk::DescriptorSetLayout layout)
{
    m_setLayouts.push_back(layout);
    return *this;
}

std::vector<vk::DescriptorSet> DescriptorSetBuilder::build(vk::Device device, vk::DescriptorPool descriptorPool)
{
    vk::DescriptorSetAllocateInfo descriptorSetAllocInfo{};
    descriptorSetAllocInfo.descriptorPool = descriptorPool;
    descriptorSetAllocInfo.setSetLayouts(m_setLayouts);
    return device.allocateDescriptorSets(descriptorSetAllocInfo);
}

//==============================================================================
// Write Descriptor Set

DescriptorSetUpdater::DescriptorSetUpdater()
    : m_dstSet{ VK_NULL_HANDLE }
{
}

DescriptorSetUpdater &DescriptorSetUpdater::beginDescriptorSet(vk::DescriptorSet dstSet)
{
    m_dstSet = dstSet;
    return *this;
}

DescriptorSetUpdater &DescriptorSetUpdater::addBuffer(
    uint32_t binding,
    vk::DescriptorType descriptorType,
    vk::DescriptorBufferInfo *bufferInfo)
{
    assert(m_dstSet != VK_NULL_HANDLE && "beginDescriptorSet() must be called first");
    vk::WriteDescriptorSet write{};
    write.dstSet = m_dstSet;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = descriptorType;
    write.descriptorCount = 1;
    write.pBufferInfo = bufferInfo;

    m_writeDescriptorSets.push_back(write);
    return *this;
}

DescriptorSetUpdater &DescriptorSetUpdater::addImage(uint32_t binding, vk::DescriptorType descriptorType, vk::DescriptorImageInfo *imageInfo)
{
    assert(m_dstSet != VK_NULL_HANDLE && "beginDescriptorSet() must be called first");
    vk::WriteDescriptorSet write{};
    write.dstSet = m_dstSet;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = descriptorType;
    write.descriptorCount = 1;
    write.pImageInfo = imageInfo;

    m_writeDescriptorSets.push_back(write);
    return *this;
}

void DescriptorSetUpdater::update(vk::Device device)
{
    std::vector<vk::CopyDescriptorSet> copySets;
    return device.updateDescriptorSets(m_writeDescriptorSets, copySets);
}
