/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "vulkan/ve_base.hpp"
#include "vulkan/ve_descriptor.hpp"
#include "vulkan/ve_renderer.hpp"

class Framework
{
public:
    Framework(
        InstanceBuilder &instanceBuilder,
        DeviceBuilder &deviceBuilder,
        DescriptorPoolBuilder &descriptorPoolBuilder,
        Window &window);

    ~Framework();

    Window &getWindow() { return m_window; }
    VulkanBase &getVulkanBase() { return m_base; }
    vk::Instance getInstance() const { return m_base.getInstance(); }
    vk::PhysicalDevice getPhysicalDevice() const { return m_base.getPhysicalDevice(); }
    vk::PhysicalDeviceMemoryProperties getMemoryProperties() const { return m_base.getMemoryProperties(); }
    vk::PhysicalDeviceFeatures getFeatures() const { return m_base.getFeatures(); }
    vk::PhysicalDeviceFeatures getEnabledFeatures() const { return m_base.getEnabledFeatures(); }
    vk::PhysicalDeviceProperties getProperties() const { return m_base.getProperties(); }
    vk::Device getDevice() const { return m_base.getDevice(); }
    vk::Queue getGraphicsQueue() { return m_base.getGraphicsQueue(); }
    vk::Queue getPresentQueue() { return m_base.getPresentQueue(); }
    uint32_t getGraphicsQueueFamilyIndex() const { return m_base.getGraphicsQueueFamilyIndex(); }
    uint32_t getPresentQueueFamilyIndex() const { return m_base.getPresentQueueFamilyIndex(); }
    vk::PipelineCache getPipelineCache() const { return m_base.getPipelineCache(); }
    vk::CommandPool getCommandPool() { return m_base.getCommandPool(); }

    vk::DescriptorPool getDescriptorPool() { return m_descriptorPool; }
    Renderer &getRenderer() { return m_renderer; }

private:
    Window &m_window;
    VulkanBase m_base;
    Renderer m_renderer;

    vk::DescriptorPool m_descriptorPool;
};