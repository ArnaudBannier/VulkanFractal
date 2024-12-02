/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_base.hpp"
#include "vulkan/ve_tools.hpp"

PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger)
{
    return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const *pAllocator)
{
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
    void *pUserData)
{
    std::string message;
    message += "[" + vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) + "] ";
    message += "Types = " + vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)) + " ";
    if (message.size() < 100) message += std::string(100 - message.size(), '-');
    message += "\n";

    message += std::string(4, ' ') + "messageIDName   = <" + pCallbackData->pMessageIdName + ">\n";
    message += std::string(4, ' ') + "messageIdNumber = " + std::to_string(pCallbackData->messageIdNumber) + "\n";
    message += std::string(4, ' ') + "message         = <" + pCallbackData->pMessage + ">\n";
    if (0 < pCallbackData->queueLabelCount)
    {
        message += std::string(4, ' ') + "Queue Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
        {
            message += std::string(8, ' ') + "labelName = <" + pCallbackData->pQueueLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount)
    {
        message += std::string(4, ' ') + "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
        {
            message += std::string(8, ' ') + "labelName = <" + pCallbackData->pCmdBufLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->objectCount)
    {
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
        {
            message += std::string(4, ' ') + "Object " + std::to_string(i) + "\n";
            message += std::string(8, ' ') + "objectType   = "
                + vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) + "\n";
            message += std::string(8, ' ') + "objectHandle = "
                + std::to_string(pCallbackData->pObjects[i].objectHandle) + "\n";
            if (pCallbackData->pObjects[i].pObjectName)
            {
                message += std::string(8, ' ') + "objectName   = <" + pCallbackData->pObjects[i].pObjectName + ">\n";
            }
        }
    }
    std::cout << message << std::endl;
    return false;
}

VulkanBase::VulkanBase(
    InstanceBuilder & instanceBuilder,
    DeviceBuilder &deviceBuilder,
    Window &window)
    : m_window{ window }
{
    //m_dynamicDispatcher = vk::DispatchLoaderDynamic();
    //m_dynamicDispatcher.init(vkGetInstanceProcAddr);

    // Create the instance
    m_instance = instanceBuilder.build();
    initDispatchLoaderStaticWithInstance();

    //m_dynamicDispatcher.init(m_instance);

    // Create the debug messenger
    if (instanceBuilder.areValidationLayersEnabled())
    {
        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCI{};
        debugMessengerCI.messageSeverity =
            //vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        debugMessengerCI.messageType =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        debugMessengerCI.pfnUserCallback = &debugMessageFunc;
        debugMessengerCI.pUserData = nullptr;

        m_debugUtilsMessenger = m_instance.createDebugUtilsMessengerEXT(debugMessengerCI);
    }

    // Create the surface
    m_window.createSurface(m_instance, &m_surface);

    // Create the physical device
    createPhysicalDevice(deviceBuilder);

    // Create the logical device
    createLogicalDevice(deviceBuilder);

    //m_dynamicDispatcher.init(m_device);

    // Create the pipeline cache
    vk::PipelineCacheCreateInfo pipelineCacheCI{};
    m_pipelineCache = m_device.createPipelineCache(pipelineCacheCI);

    // Create the command pool
    createCommandPool();
}

VulkanBase::~VulkanBase()
{
    m_device.destroyCommandPool(m_commandPool);
    m_device.destroyPipelineCache(m_pipelineCache);
    m_device.destroy();

    m_instance.destroySurfaceKHR(m_surface);
    m_instance.destroyDebugUtilsMessengerEXT(m_debugUtilsMessenger);
    m_instance.destroy();
}

void VulkanBase::createPhysicalDevice(DeviceBuilder &deviceBuilder)
{
    std::vector<vk::PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();
    std::cout << "Device count: " << physicalDevices.size() << std::endl;

    m_physicalDevice = VK_NULL_HANDLE;
    uint32_t selectedGraphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t selectedPresentQueueFamilyIndex = UINT32_MAX;
    for (const vk::PhysicalDevice &physicalDevice : physicalDevices)
    {
        if (checkPhysicalDeviceProperties(
            physicalDevice,
            deviceBuilder.getDesiredExtensions(),
            deviceBuilder.getDesiredFeatures(),
            selectedGraphicsQueueFamilyIndex,
            selectedPresentQueueFamilyIndex))
        {
            m_physicalDevice = physicalDevice;
            m_graphicsQueueFamilyIndex = selectedGraphicsQueueFamilyIndex;
            m_presentQueueFamilyIndex = selectedPresentQueueFamilyIndex;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    m_properties = m_physicalDevice.getProperties();
    m_memoryProperties = m_physicalDevice.getMemoryProperties();
    m_features = m_physicalDevice.getFeatures();
    std::cout << "Physical device: " << m_properties.deviceName << std::endl;
}

bool VulkanBase::checkPhysicalDeviceProperties(
    vk::PhysicalDevice physicalDevice,
    const std::vector<const char *> &desiredExtensions,
    const vk::PhysicalDeviceFeatures &desiredFeatures,
    uint32_t &selectedGraphicsQueueFamilyIndex,
    uint32_t &selectedPresentQueueFamilyIndex)
{
    // Check the extensions
    std::vector<vk::ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    for (const char *extensionName : desiredExtensions)
    {
        if (tools::CheckExtensionAvailability(extensionName, availableExtensions) == false)
        {
            std::cout << "Physical device " << physicalDevice
                << " doesn't support extension named \"" << extensionName << "\"!" << std::endl;
            return false;
        }
    }

    vk::PhysicalDeviceProperties deviceProperties = physicalDevice.getProperties();

    // Check the features
    vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice.getFeatures();
    VkBool32 *deviceFeaturesArray = (VkBool32 *)&VkPhysicalDeviceFeatures(deviceFeatures);
    VkBool32 *desiredFeaturesArray = (VkBool32 *)&VkPhysicalDeviceFeatures(desiredFeatures);
    for (int i = 0; i < sizeof(deviceFeatures) / sizeof(VkBool32); i++)
    {
        if (desiredFeaturesArray[i] && (deviceFeaturesArray[i] == VK_FALSE))
        {
            return false;
        }
    }

    // Check swapchain support
    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(m_surface);
    std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(m_surface);

    if (surfaceFormats.empty() || presentModes.empty())
    {
        return false;
    }

    // Select the graphics and present queue famillies
    std::vector<vk::QueueFamilyProperties> queueFamillyProperties = physicalDevice.getQueueFamilyProperties();
    uint32_t queueFamilyCount = static_cast<uint32_t>(queueFamillyProperties.size());
    if (queueFamilyCount == 0)
    {
        return false;
    }

    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentQueueFamilyIndex = UINT32_MAX;
    std::vector<vk::Bool32> queuePresentSupport(queueFamillyProperties.size());
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        queuePresentSupport[i] = physicalDevice.getSurfaceSupportKHR(i, m_surface);

        if ((queueFamillyProperties[i].queueCount > 0) &&
            (queueFamillyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics))
        {
            // Select first queue that supports graphics
            if (graphicsQueueFamilyIndex == UINT32_MAX)
            {
                graphicsQueueFamilyIndex = i;
            }

            // If there is queue that supports both graphics and present - prefer it
            if (queuePresentSupport[i])
            {
                selectedGraphicsQueueFamilyIndex = i;
                selectedPresentQueueFamilyIndex = i;
                return true;
            }
        }
    }

    // We don't have queue that supports both graphics and present so we have to use separate queues
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if (queuePresentSupport[i])
        {
            presentQueueFamilyIndex = i;
            break;
        }
    }

    // If this device doesn't support queues with graphics and present capabilities don't use it
    if ((graphicsQueueFamilyIndex == UINT32_MAX) ||
        (presentQueueFamilyIndex == UINT32_MAX))
    {
        std::cout << "Could not find queue families with required properties on physical device "
            << physicalDevice << "!" << std::endl;
        return false;
    }

    selectedGraphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
    selectedPresentQueueFamilyIndex = presentQueueFamilyIndex;
    return true;
}

void VulkanBase::createLogicalDevice(DeviceBuilder &deviceBuilder)
{
    // Add the queue families
    deviceBuilder.addQueue(m_graphicsQueueFamilyIndex);
    if (m_presentQueueFamilyIndex != m_graphicsQueueFamilyIndex)
        deviceBuilder.addQueue(m_presentQueueFamilyIndex);

    // Create the logical device
    m_device = deviceBuilder.build(m_physicalDevice);
    m_enabledFeatures = deviceBuilder.getDesiredFeatures();

    m_graphicsQueue = m_device.getQueue(m_graphicsQueueFamilyIndex, 0);
    m_presentQueue = m_device.getQueue(m_presentQueueFamilyIndex, 0);
}

void VulkanBase::createCommandPool()
{
    vk::CommandPoolCreateInfo commandPoolCI{};
    commandPoolCI.flags =
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
        vk::CommandPoolCreateFlagBits::eTransient;
    commandPoolCI.queueFamilyIndex = m_graphicsQueueFamilyIndex;

    m_commandPool = m_device.createCommandPool(commandPoolCI);
}

void VulkanBase::initDispatchLoaderStaticWithInstance()
{
    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        m_instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT)
    {
        throw std::runtime_error(
            "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");
    }

    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        m_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT)
    {
        throw std::runtime_error(
            "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");
    }
}
