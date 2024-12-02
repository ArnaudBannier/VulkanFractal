/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "vulkan/ve_device.hpp"

//==============================================================================
// Descriptor Pool

class DescriptorPoolBuilder
{
public:
    DescriptorPoolBuilder();
    DescriptorPoolBuilder &addPoolSize(vk::DescriptorType descriptorType, uint32_t count);
    DescriptorPoolBuilder &setPoolFlags(vk::DescriptorPoolCreateFlags flags);
    DescriptorPoolBuilder &setMaxSets(uint32_t count);

    vk::DescriptorPool build(vk::Device device);
private:

    std::vector<vk::DescriptorPoolSize> m_poolSizes{};
    uint32_t m_maxSets = 256;
    vk::DescriptorPoolCreateFlags m_poolFlags;
};

//==============================================================================
// Descriptor Set Layout

class DescriptorSetLayoutBuilder
{
public:
    DescriptorSetLayoutBuilder() {}

    DescriptorSetLayoutBuilder &addBinding(
        uint32_t binding,
        vk::DescriptorType descriptorType,
        vk::ShaderStageFlags stageFlags,
        uint32_t count = 1);

    vk::DescriptorSetLayout build(vk::Device device);

private:
    std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> m_bindings{};
};

//==============================================================================
// Descriptor Set

class DescriptorSetBuilder
{
public:
    DescriptorSetBuilder() {}

    DescriptorSetBuilder &addLayout(vk::DescriptorSetLayout layout);

    std::vector<vk::DescriptorSet> build(vk::Device device, vk::DescriptorPool descriptorPool);

private:
    std::vector<vk::DescriptorSetLayout> m_setLayouts;
};

//==============================================================================
// Write Descriptor Set

class DescriptorSetUpdater
{
public:
    DescriptorSetUpdater();

    DescriptorSetUpdater &beginDescriptorSet(vk::DescriptorSet dstSet);

    DescriptorSetUpdater &addBuffer(
        uint32_t binding,
        vk::DescriptorType descriptorType,
        vk::DescriptorBufferInfo *bufferInfo);

    DescriptorSetUpdater &addImage(
        uint32_t binding,
        vk::DescriptorType descriptorType,
        vk::DescriptorImageInfo *imageInfo);

    void update(vk::Device device);

private:
    vk::DescriptorSet m_dstSet;
    std::vector<vk::WriteDescriptorSet> m_writeDescriptorSets;
};
