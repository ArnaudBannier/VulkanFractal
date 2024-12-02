/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"

class VulkanBase;

namespace tools
{
	uint32_t alignedSize(uint32_t value, uint32_t alignment);
	size_t alignedSize(size_t value, size_t alignment);
	vk::DeviceSize alignedVkSize(vk::DeviceSize value, vk::DeviceSize alignment);

	vk::PipelineShaderStageCreateInfo loadShader(
		vk::Device device, std::string filename, vk::ShaderStageFlagBits shaderStageFlag);

	/// Utility function for finding memory types for uniforms and images.
	uint32_t findMemoryTypeIndex(
		const vk::PhysicalDeviceMemoryProperties &memprops,
		uint32_t memoryTypeBits,
		vk::MemoryPropertyFlags search);

	bool CheckExtensionAvailability(
		const char *extensionName,
		const std::vector<vk::ExtensionProperties> &availableExtensions);

	bool CheckLayerAvailability(
		const char *layerName,
		const std::vector<vk::LayerProperties> &availableLayers);


	vk::CommandBuffer beginSingleTimeCommands(
		vk::Device device,
		vk::CommandPool commandPool);

	vk::CommandBuffer beginSingleTimeCommands(VulkanBase &base);

	void endSingleTimeCommands(
		vk::Device device,
		vk::Queue queue,
		vk::CommandPool commandPool,
		vk::CommandBuffer commandBuffer);

	void endSingleTimeCommands(VulkanBase &base, vk::CommandBuffer commandBuffer);

	uint32_t getMipSize(uint32_t baseSize, uint32_t mipLevel);
}