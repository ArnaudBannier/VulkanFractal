/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ve_instance.hpp"
#include "ve_tools.hpp"

InstanceBuilder::InstanceBuilder()
    : m_applicationInfo{}
{
    m_applicationInfo.pApplicationName = "Vulkan application";
    m_applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    m_applicationInfo.pEngineName = "Vulkan.hpp";
    m_applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    m_applicationInfo.apiVersion = VK_API_VERSION_1_0;
}

InstanceBuilder &InstanceBuilder::addLayer(const char *layer)
{
    m_layers.push_back(layer);
    return *this;
}

InstanceBuilder &InstanceBuilder::addExtension(const char *extension)
{
    m_extensions.push_back(extension);
    return *this;
}

InstanceBuilder &InstanceBuilder::addSDLExtensions(SDL_Window *window)
{
    std::vector<const char *> extensions;
    uint32_t count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
    extensions.resize(count);
    SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());
    for (const char *ext : extensions)
    {
        m_extensions.push_back(ext);
    }
    return *this;
}

InstanceBuilder &InstanceBuilder::setApplicationName(const char *applicationName)
{
    m_applicationInfo.pApplicationName = applicationName;
    return *this;
}

InstanceBuilder &InstanceBuilder::setApplicationVersion(uint32_t applicationVersion)
{
    m_applicationInfo.applicationVersion = applicationVersion;
    return *this;
}

InstanceBuilder &InstanceBuilder::enableValidation()
{
    m_enableValidationLayers = true;
    m_layers.push_back("VK_LAYER_KHRONOS_validation");
    m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return *this;
}

vk::Instance InstanceBuilder::build()
{
    // Check the layers
    std::cout << "Requested layers:" << std::endl;
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
    for (const char *layerName : m_layers)
    {
        if (tools::CheckLayerAvailability(layerName, availableLayers) == false)
        {
            std::cout << "[ERROR] the layer " << layerName << " is not available\n";
            throw std::runtime_error("the layer " + std::string(layerName) + " is not available");
        }
        std::cout << "- " << layerName << std::endl;
    }
    // Check the extensions
    std::cout << "Requested extensions:" << std::endl;
    std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();
    for (const char *extensionName : m_extensions)
    {
        if (tools::CheckExtensionAvailability(extensionName, availableExtensions) == false)
        {
            std::cout << "[ERROR] the extension " << extensionName << " is not available\n";
            throw std::runtime_error("the extension " + std::string(extensionName) + " is not available");
        }
        std::cout << "- " << extensionName << std::endl;
    }

    vk::InstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.pApplicationInfo = &m_applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = m_extensions.data();
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_layers.size());
    instanceCreateInfo.ppEnabledLayerNames = m_layers.data();
    instanceCreateInfo.pNext = nullptr;

    return vk::createInstance(instanceCreateInfo);
}
