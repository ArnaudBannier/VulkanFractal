/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"

class InstanceBuilder
{
public:
    InstanceBuilder();

    InstanceBuilder &addLayer(const char *layer);
    InstanceBuilder &addExtension(const char *extension);

    InstanceBuilder &addSDLExtensions(SDL_Window *window);
    InstanceBuilder &setApplicationName(const char *applicationName);
    InstanceBuilder &setApplicationVersion(uint32_t applicationVersion);

    InstanceBuilder &enableValidation();

    vk::Instance build();

    bool areValidationLayersEnabled() const { return m_enableValidationLayers; }

private:
    vk::ApplicationInfo m_applicationInfo;

    bool m_enableValidationLayers = true;
    const std::vector<const char *> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char *> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    std::vector<const char *> m_layers;
    std::vector<const char *> m_extensions;
};
