/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_device.hpp"
#include "vulkan/ve_tools.hpp"
#include "ve_instance.hpp"

DeviceBuilder::DeviceBuilder()
    : m_features{}
{
}

DeviceBuilder &DeviceBuilder::addLayer(const char *layer)
{
    m_layers.push_back(layer);
    return *this;
}

DeviceBuilder &DeviceBuilder::addExtension(const char *extension)
{
    m_extensions.push_back(extension);
    return *this;
}

DeviceBuilder &DeviceBuilder::setFeatures(const vk::PhysicalDeviceFeatures &features)
{
    m_features = features;
    return *this;
}

DeviceBuilder &DeviceBuilder::addQueue(uint32_t familyIndex, float priority, uint32_t n)
{
    m_queuePriorities.emplace_back(n, priority);

    vk::DeviceQueueCreateInfo queueCI{};
    queueCI.queueFamilyIndex = familyIndex;
    queueCI.queueCount = n;
    queueCI.pQueuePriorities = m_queuePriorities.back().data();
    m_queues.push_back(queueCI);
    return *this;
}

DeviceBuilder &DeviceBuilder::enableGeometryShader()
{
    m_features.geometryShader = vk::True;
    return *this;
}

DeviceBuilder &DeviceBuilder::enableTesselationShader()
{
    m_features.tessellationShader = vk::True;
    return *this;
}

DeviceBuilder &DeviceBuilder::enableShaderFloat64()
{
    m_features.shaderFloat64 = vk::True;
    return *this;
}

DeviceBuilder &DeviceBuilder::enableSamplerAnisotropy()
{
    m_features.samplerAnisotropy = vk::True;
    return *this;
}

DeviceBuilder &DeviceBuilder::enableFillModeNonSolid()
{
    m_features.fillModeNonSolid = vk::True;
    return *this;
}

vk::Device DeviceBuilder::build(vk::PhysicalDevice physicalDevice)
{
    vk::DeviceCreateInfo deviceCI{};

    deviceCI.pEnabledFeatures = &m_features;

    deviceCI.queueCreateInfoCount = static_cast<uint32_t>(m_queues.size());
    deviceCI.pQueueCreateInfos = m_queues.data();

    deviceCI.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
    deviceCI.ppEnabledExtensionNames = m_extensions.data();

    deviceCI.enabledLayerCount = static_cast<uint32_t>(m_layers.size());
    deviceCI.ppEnabledLayerNames = m_layers.data();

    return physicalDevice.createDevice(deviceCI);
}
