/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_framework.hpp"
#include "vulkan/ve_tools.hpp"

Framework::Framework(
    InstanceBuilder &instanceBuilder,
    DeviceBuilder &deviceBuilder,
    DescriptorPoolBuilder &descriptorPoolBuilder,
    Window &window)
    : m_base{ instanceBuilder, deviceBuilder, window }
    , m_renderer{ m_base, (vk::Extent2D)(window.getExtent()) }
    , m_window{ window }
{
    // Create the descriptor pool
    m_descriptorPool = descriptorPoolBuilder.build(m_base.getDevice());
}

Framework::~Framework()
{
    m_base.getDevice().destroyDescriptorPool(m_descriptorPool);
}

