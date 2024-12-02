/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "vulkan/ve_instance.hpp"
#include "vulkan/ve_device.hpp"
#include "vulkan/ve_window.hpp"

class VulkanBase
{
public:
    VulkanBase(
        InstanceBuilder &instanceBuilder,
        DeviceBuilder &deviceBuilder,
        Window &window);

    ~VulkanBase();

    vk::Instance getInstance() const { return m_instance; }
    vk::SurfaceKHR getSurface() const { return m_surface; }
    vk::PhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    vk::PhysicalDeviceMemoryProperties getMemoryProperties() const { return m_memoryProperties; }
    vk::PhysicalDeviceFeatures getFeatures() const { return m_features; }
    vk::PhysicalDeviceFeatures getEnabledFeatures() const { return m_enabledFeatures; }
    vk::PhysicalDeviceProperties getProperties() const { return m_properties; }
    vk::Device getDevice() const { return m_device; }
    vk::Queue getGraphicsQueue() { return m_graphicsQueue; }
    vk::Queue getPresentQueue() { return m_presentQueue; }
    uint32_t getGraphicsQueueFamilyIndex() const { return m_graphicsQueueFamilyIndex; }
    uint32_t getPresentQueueFamilyIndex() const { return m_presentQueueFamilyIndex; }
    vk::PipelineCache getPipelineCache() const { return m_pipelineCache; }

    vk::CommandPool getCommandPool() { return m_commandPool; }

private:
    void createPhysicalDevice(DeviceBuilder &deviceBuilder);
    bool checkPhysicalDeviceProperties(
        vk::PhysicalDevice physicalDevice,
        const std::vector<const char *> &desiredExtensions,
        const vk::PhysicalDeviceFeatures &desiredFeatures,
        uint32_t &selectedGraphicsQueueFamilyIndex,
        uint32_t &selectedPresentQueueFamilyIndex);
    void createLogicalDevice(DeviceBuilder &deviceBuilder);
    void createCommandPool();
    void initDispatchLoaderStaticWithInstance();

    Window &m_window;

    vk::Instance m_instance;
    vk::SurfaceKHR m_surface;
    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_device;

    //vk::DispatchLoaderDynamic m_dynamicDispatcher;
    vk::DebugUtilsMessengerEXT m_debugUtilsMessenger;

    vk::PipelineCache m_pipelineCache;
    vk::PhysicalDeviceMemoryProperties m_memoryProperties;

    uint32_t m_graphicsQueueFamilyIndex;
    uint32_t m_presentQueueFamilyIndex;
    vk::Queue m_graphicsQueue;
    vk::Queue m_presentQueue;

    vk::PhysicalDeviceProperties m_properties;
    vk::PhysicalDeviceFeatures m_features;
    vk::PhysicalDeviceFeatures m_enabledFeatures;

    vk::CommandPool m_commandPool;
};