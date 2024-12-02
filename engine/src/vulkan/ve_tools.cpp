/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_tools.hpp"
#include "vulkan/ve_base.hpp"
#include <fstream>
#include <iostream>

uint32_t tools::alignedSize(uint32_t value, uint32_t alignment)
{
    if (alignment > 0) return (value + alignment - 1) & ~(alignment - 1);
    else return value;
}

size_t tools::alignedSize(size_t value, size_t alignment)
{
    if (alignment > 0) return (value + alignment - 1) & ~(alignment - 1);
    else return value;
}

vk::DeviceSize tools::alignedVkSize(vk::DeviceSize value, vk::DeviceSize alignment)
{
    if (alignment > 0) return (value + alignment - 1) & ~(alignment - 1);
    else return value;
}

vk::PipelineShaderStageCreateInfo tools::loadShader(vk::Device device, std::string filename, vk::ShaderStageFlagBits shaderStageFlag)
{
	vk::PipelineShaderStageCreateInfo shaderStageCI{};
	shaderStageCI.stage = shaderStageFlag;
	shaderStageCI.pName = "main";
	std::ifstream is(filename, std::ios::binary | std::ios::in | std::ios::ate);

	if (is.is_open()) {
		size_t size = is.tellg();
		is.seekg(0, std::ios::beg);
		char* shaderCode = new char[size];
		is.read(shaderCode, size);
		is.close();
		assert(size > 0);

		VkShaderModuleCreateInfo moduleCreateInfo{};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.codeSize = size;
		moduleCreateInfo.pCode = (uint32_t*)shaderCode;

		shaderStageCI.module = device.createShaderModule(moduleCreateInfo);
		delete[] shaderCode;
	}
	else {
		std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
		shaderStageCI.module = VK_NULL_HANDLE;
	}

	assert(shaderStageCI.module != VK_NULL_HANDLE);
	return shaderStageCI;
}

uint32_t tools::findMemoryTypeIndex(
    const vk::PhysicalDeviceMemoryProperties &memprops,
    uint32_t memoryTypeBits,
    vk::MemoryPropertyFlags search)
{
    for (uint32_t i = 0; i < memprops.memoryTypeCount; i++)
    {
        if ((memoryTypeBits & (1 << i)) &&
            (memprops.memoryTypes[i].propertyFlags & search) == search)
        {
            return i;
        }
    }
    assert(false && "Unable to find the memory type index");
    return -1;
}

bool tools::CheckExtensionAvailability(
    const char *extensionName, const std::vector<vk::ExtensionProperties> &availableExtensions)
{
    for (const auto &extensionProperties : availableExtensions)
    {
        if (strcmp(extensionName, extensionProperties.extensionName) == 0)
        {
            return true;
        }
    }
    return false;
}

bool tools::CheckLayerAvailability(
    const char *layerName, const std::vector<vk::LayerProperties> &availableLayers)
{
    for (const auto &layerProperties : availableLayers)
    {
        if (strcmp(layerName, layerProperties.layerName) == 0)
        {
            return true;
        }
    }
    return false;
}


vk::CommandBuffer tools::beginSingleTimeCommands(vk::Device device, vk::CommandPool commandPool)
{
    vk::CommandBufferAllocateInfo commandBufferAllocInfo{
        commandPool, vk::CommandBufferLevel::ePrimary, 1
    };
    vk::CommandBuffer commandBuffer;
    vk::Result result = device.allocateCommandBuffers(&commandBufferAllocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    commandBuffer.begin(commandBufferBeginInfo);
    return commandBuffer;
}

vk::CommandBuffer tools::beginSingleTimeCommands(VulkanBase &base)
{
    return beginSingleTimeCommands(base.getDevice(), base.getCommandPool());
}

void tools::endSingleTimeCommands(
    vk::Device device,
    vk::Queue queue,
    vk::CommandPool commandPool,
    vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vk::Result result = queue.submit(1, &submitInfo, VK_NULL_HANDLE);
    queue.waitIdle();

    device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void tools::endSingleTimeCommands(VulkanBase &base, vk::CommandBuffer commandBuffer)
{
    endSingleTimeCommands(
        base.getDevice(), base.getGraphicsQueue(), base.getCommandPool(),
        commandBuffer);
}

uint32_t tools::getMipSize(uint32_t baseSize, uint32_t mipLevel)
{
    uint32_t size = baseSize >> mipLevel;
    return size > 1 ? size : 1;
}
