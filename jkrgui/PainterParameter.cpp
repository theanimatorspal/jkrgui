#include "PainterParameter.hpp"
extern "C" {
#include <Vendor/stbi/stb_image.h>
}

void Jkr::PainterParameterBase::Setup(Up<StorageBufferType>& inStorageBuffer, vk::DeviceSize inDeviceSize)
{
	inStorageBuffer = MakeUp<VulkanBufferVMA<BufferContext::Storage, MemoryType::DeviceLocal>>(mInstance.GetVMA(), mInstance.GetDevice(), inDeviceSize);
}

void Jkr::PainterParameterBase::Setup(Up<UniformBufferType>& inUniformBuffer, vk::DeviceSize inDeviceSize, void** inMappedMemoryRegion)
{
	inUniformBuffer = MakeUp<VulkanBufferVMA<BufferContext::Uniform, MemoryType::HostVisibleAndCoherenet>>(mInstance.GetVMA(), mInstance.GetDevice(), inDeviceSize), inUniformBuffer->MapMemoryRegion(inMappedMemoryRegion);
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inStorageImageSampler, Up<StorageImageType>& inStorageImage, uint32_t inWidth, uint32_t inHeight)
{
	inStorageImage = MakeUp<VulkanImageVMA<ImageContext::Storage>>(mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight);
	inStorageImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, std::string inFileName)
{
	int Width;
	int Height;
	int Channels;
	void* data = stbi_load(inFileName.c_str(), &Width, &Height, &Channels, STBI_rgb_alpha);
	inUniformImage = MakeUp<VulkanImageVMA<ImageContext::Default>>(mInstance.GetVMA(), mInstance.GetDevice(), Width, Height);
	inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());

	inUniformImage->SubmitImmediateCmdCopyFromData(
		mInstance.GetGraphicsQueue(),
		mInstance.GetUtilCommandBuffer(),
		mInstance.GetDevice(),
		&data,
		4 * Width * Height);
	stbi_image_free(data);
}

void Jkr::PainterParameterBase::Setup(Up<VulkanSampler>& inUniformImageSampler, Up<UniformImageType>& inUniformImage, void** inData, uint32_t inWidth, uint32_t inHeight, uint32_t inChannelCount)
{
	inUniformImage = MakeUp<VulkanImageVMA<ImageContext::Default>>(mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight, inChannelCount);
	inUniformImageSampler = MakeUp<VulkanSampler>(mInstance.GetDevice());
	inUniformImage->SubmitImmediateCmdCopyFromData(
		mInstance.GetGraphicsQueue(),
		mInstance.GetUtilCommandBuffer(),
		mInstance.GetDevice(),
		inData,
		inChannelCount * inWidth * inHeight
	);
}

