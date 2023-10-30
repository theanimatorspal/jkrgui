#include "GUIPrimitive.hpp"

Jkr::GUIPrimitive::GUIPrimitive(const Instance& inInstance)
	: mInstance(inInstance)
{
}

Jkr::GUIPrimitive::GUIPrimitive(
	const Instance& inInstance,
	const vk::ArrayProxyNoTemporaries<kstd::Vertex>& inVertices,
	const vk::ArrayProxyNoTemporaries<kstd::ui32>& inIndices
)
	: mInstance(inInstance), mIndexCount(inIndices.size())
{
	mVertexBufferPtr = MakeUp<VulkanBufferVMA<BufferContext::Vertex, MemoryType::DeviceLocal>>(inInstance.GetVMA(), mInstance.GetDevice(), vk::DeviceSize(sizeof(inVertices.front()) * inVertices.size()));
	mIndexBufferPtr = MakeUp<VulkanBufferVMA<BufferContext::Index, MemoryType::DeviceLocal>>(inInstance.GetVMA(), mInstance.GetDevice(), vk::DeviceSize(sizeof(uint32_t) * inIndices.size()));
	mVertexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(), mInstance.GetUtilCommandBuffer(), reinterpret_cast<void*>(inVertices.data()));
	mIndexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(), mInstance.GetUtilCommandBuffer(), reinterpret_cast<void*>(inIndices.data()));
	mInstance.GetGraphicsQueue().Wait();
}

Jkr::GUIPrimitive::GUIPrimitive(const Instance& inInstance, const vk::ArrayProxyNoTemporaries<kstd::VertexEXT>& inVertices, const vk::ArrayProxyNoTemporaries<kstd::ui32>& inIndices, int inExtParameter_DoesNotDoAnything)
	: mInstance(inInstance), mIndexCount(inIndices.size())
{
	mVertexBufferPtr = MakeUp<VulkanBufferVMA<BufferContext::Vertex, MemoryType::DeviceLocal>>(inInstance.GetVMA(), mInstance.GetDevice(), vk::DeviceSize(sizeof(inVertices.front()) * inVertices.size()));
	mIndexBufferPtr = MakeUp<VulkanBufferVMA<BufferContext::Index, MemoryType::DeviceLocal>>(inInstance.GetVMA(), mInstance.GetDevice(), vk::DeviceSize(sizeof(uint32_t) * inIndices.size()));
	mVertexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(), mInstance.GetUtilCommandBuffer(), reinterpret_cast<void*>(inVertices.data()));
	mIndexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(), mInstance.GetUtilCommandBuffer(), reinterpret_cast<void*>(inIndices.data()));
	mInstance.GetGraphicsQueue().Wait();
}

Jkr::GUIPrimitive::GUIPrimitive(const Instance& inInstance, vk::DeviceSize inVertexBufferSizeInBytes, vk::DeviceSize inIndexBufferSizeInBytes)
	: mInstance(inInstance), mIndexCount(0)
{
	mVertexBufferPtr = MakeUp<VulkanBufferVMA<BufferContext::Vertex, MemoryType::DeviceLocal>>(inInstance.GetVMA(), mInstance.GetDevice(), inVertexBufferSizeInBytes);
	mIndexBufferPtr = MakeUp<VulkanBufferVMA<BufferContext::Index, MemoryType::DeviceLocal>>(inInstance.GetVMA(), mInstance.GetDevice(), inIndexBufferSizeInBytes);
}

