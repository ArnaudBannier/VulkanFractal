/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "vulkan/ve_window.hpp"

class DeviceBuilder
{
public:
    DeviceBuilder();

    DeviceBuilder &addLayer(const char *layer);
    DeviceBuilder &addExtension(const char *extension);
    DeviceBuilder &setFeatures(const vk::PhysicalDeviceFeatures &features);
    DeviceBuilder &addQueue(uint32_t familyIndex, float priority = 0.0f, uint32_t n = 1);

    DeviceBuilder &enableGeometryShader();
    DeviceBuilder &enableTesselationShader();
    DeviceBuilder &enableShaderFloat64();
    DeviceBuilder &enableSamplerAnisotropy();
    DeviceBuilder &enableFillModeNonSolid();

    vk::Device build(vk::PhysicalDevice physicalDevice);

    const std::vector<const char *> &getDesiredLayers() const { return m_layers; }
    const std::vector<const char *> &getDesiredExtensions() const { return m_extensions; }
    const vk::PhysicalDeviceFeatures &getDesiredFeatures() const { return m_features; }

private:
    std::vector<const char *> m_layers;
    std::vector<const char *> m_extensions;
    std::vector<vk::DeviceQueueCreateInfo> m_queues;
    std::vector<std::vector<float> > m_queuePriorities;
    vk::PhysicalDeviceFeatures m_features;
};
