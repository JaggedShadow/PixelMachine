#include "VlkBuffer.h"
#include "VlkDevice.h"
#include "VlkRenderContext.h"

#include <stdexcept>

namespace PixelMachine {
	namespace GPU {

		/* Create a VkBuffer with VkDeviceMemory allocation - Returns handle to a new buffer (VK_NULL_HANDLE if failed) */
		static VkBuffer CreateVkBuffer(
			const uint32_t size,
			const uint32_t usageFlagBits,
			const uint32_t memoryPropertyFlagBits,
			VkDeviceMemory &outDeviceMemory) {

			VkBuffer buffer = VK_NULL_HANDLE;
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usageFlagBits;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VlkDevice *deviceP = VlkRenderContext::GetVlkDevice();

			vkCreateBuffer(deviceP->GetHandle(), &bufferInfo, nullptr, &buffer);

			VkMemoryRequirements memoryRequirements = {};
			vkGetBufferMemoryRequirements(deviceP->GetHandle(), buffer, &memoryRequirements);

			VkPhysicalDeviceMemoryProperties bufferMemoryProps = {};
			vkGetPhysicalDeviceMemoryProperties(deviceP->GetActiveAdapter().GetHandle(), &bufferMemoryProps);

			int memoryTypeIndex = -1;
			for (int i = 0; i < bufferMemoryProps.memoryTypeCount; i++) {
				if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
					(bufferMemoryProps.memoryTypes[i].propertyFlags & memoryPropertyFlagBits)) {
					memoryTypeIndex = i;
					break;
				}
			}

			if (memoryTypeIndex == -1) {
				vkDestroyBuffer(deviceP->GetHandle(), buffer, nullptr);
				return buffer;
			}

			VkMemoryAllocateInfo memoryAllocateInfo = {};
			memoryAllocateInfo.allocationSize = memoryRequirements.size;
			memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

			VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
			vkAllocateMemory(deviceP->GetHandle(), &memoryAllocateInfo, nullptr, &bufferMemory);

			if (!bufferMemory) {
				vkDestroyBuffer(deviceP->GetHandle(), buffer, nullptr);
				return buffer;
			}

			vkBindBufferMemory(deviceP->GetHandle(), buffer, bufferMemory, 0);

			outDeviceMemory = bufferMemory;

			return buffer;
		}

		static void ReleaseVkBuffer(VkBuffer &buffer, VkDeviceMemory &deviceMemory) {

			VkDevice device = PixelMachine::GPU::VlkRenderContext::GetVlkDevice()->GetHandle();

			vkDeviceWaitIdle(device);

			if (deviceMemory != VK_NULL_HANDLE) {
				vkFreeMemory(device, deviceMemory, nullptr);
				deviceMemory == VK_NULL_HANDLE;
			}

			if (buffer != VK_NULL_HANDLE) {
				vkDestroyBuffer(device, buffer, nullptr);
				buffer == VK_NULL_HANDLE;
			}

		}

		static VkBufferUsageFlagBits GetVkBufferUsage(BufferType bufferType) {
			switch (bufferType)
			{
			case PixelMachine::GPU::VertexBuffer:		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case PixelMachine::GPU::IndexBuffer:		return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case PixelMachine::GPU::UniformBuffer:		return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			default: break;
			}
		}

		Buffer *Buffer::Create(
			const BufferType type,
			const ShaderProgramType bindStage,
			const BufferLayout dataLayout,
			const uint32_t elementCount) {

			if (type == BufferType::UniformBuffer) {
				return new PixelMachine::GPU::VlkBuffer(type, bindStage, dataLayout, elementCount);
			}

			return new PixelMachine::GPU::VlkStagingBuffer(type, bindStage, dataLayout, elementCount);

		}

		VlkBuffer::VlkBuffer(const BufferType type, const ShaderProgramType bindStage, const BufferLayout dataLayout, const uint32_t elementCount)
			: Buffer(type, bindStage, dataLayout), m_size(dataLayout.GetSize() * elementCount) {

			m_vkHostBuffer = CreateVkBuffer(
				m_size,
				GetVkBufferUsage(m_type),
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_vkHostMemory);

			if (!m_vkHostBuffer) {
				throw new std::runtime_error("VlkBuffer creation failed.");
			}

			VlkDevice *vlkDeviceP = VlkRenderContext::GetVlkDevice();

			vkMapMemory(vlkDeviceP->GetHandle(), m_vkHostMemory, 0, m_size, 0, &m_mappedDataP);
		}

		VlkBuffer::VlkBuffer(const BufferType type, const ShaderProgramType bindStage, const BufferLayout dataLayout) : Buffer(type, bindStage, dataLayout) {}

		VlkBuffer::~VlkBuffer() {
			const VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();
			vkUnmapMemory(device, m_vkHostMemory);
			ReleaseVkBuffer(m_vkHostBuffer, m_vkHostMemory);
		}

		void VlkBuffer::SetData(const void *data) {
			memcpy(m_mappedDataP, data, m_size);
		}

		void VlkBuffer::Bind() const {
			VlkRenderContext *pVlkRenderContext = static_cast<VlkRenderContext *>(VlkRenderContext::Get());
			pVlkRenderContext->BindBuffer(this);
		}

		VlkStagingBuffer::VlkStagingBuffer(const BufferType type, const ShaderProgramType bindStage, const BufferLayout dataLayout, const uint32_t elementCount)
			: VlkBuffer(type, bindStage, dataLayout) {

			m_size = dataLayout.GetSize() * elementCount;

			m_vkHostBuffer = CreateVkBuffer(
				m_size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_vkHostMemory);

			if (!m_vkHostBuffer) {
				throw new std::runtime_error("VlkStagingBuffer creation failed.");
			}

			VlkDevice *vlkDeviceP = VlkRenderContext::GetVlkDevice();

			vkMapMemory(vlkDeviceP->GetHandle(), m_vkHostMemory, 0, m_size, 0, &m_mappedDataP);

			m_vkGpuBuffer = CreateVkBuffer(
				m_size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | GetVkBufferUsage(m_type),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_vkGpuMemory);

			if (!m_vkGpuBuffer) {
				throw new std::runtime_error("VlkStagingBuffer creation failed.");
			}

			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = vlkDeviceP->GetActiveQueue().second;

			vkCreateCommandPool(vlkDeviceP->GetHandle(), &cmdPoolInfo, nullptr, &m_vkCommandPool);

			if (!m_vkCommandPool) {
				throw new std::runtime_error("VlkStagingBuffer creation failed.");
			}

			VkCommandBufferAllocateInfo cmdBufferInfo = {};
			cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufferInfo.commandPool = m_vkCommandPool;
			cmdBufferInfo.commandBufferCount = 1u;

			vkAllocateCommandBuffers(vlkDeviceP->GetHandle(), &cmdBufferInfo, &m_vkCommandBuffer);

			if (!m_vkCommandBuffer) {
				throw new std::runtime_error("VlkStagingBuffer creation failed.");
			}

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

			vkCreateFence(vlkDeviceP->GetHandle(), &fenceInfo, nullptr, &m_vkFence);

			if (!m_vkFence) {
				throw new std::runtime_error("VlkStagingBuffer creation failed.");
			}

			VkCommandBufferBeginInfo bufferBeginInfo = {};
			bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			vkBeginCommandBuffer(m_vkCommandBuffer, &bufferBeginInfo);

			VkBufferCopy bufferCopy = {};
			bufferCopy.dstOffset = 0;
			bufferCopy.srcOffset = 0;
			bufferCopy.size = m_size;

			vkCmdCopyBuffer(m_vkCommandBuffer, m_vkHostBuffer, m_vkGpuBuffer, 1u, &bufferCopy);
			vkEndCommandBuffer(m_vkCommandBuffer);
		}

		VlkStagingBuffer::~VlkStagingBuffer() {

			VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();

			if (m_vkFence) {
				vkDestroyFence(device, m_vkFence, nullptr);
			}

			if (m_vkCommandBuffer) {
				vkFreeCommandBuffers(device, m_vkCommandPool, 1u, &m_vkCommandBuffer);
			}

			if (m_vkCommandPool) {
				vkDestroyCommandPool(device, m_vkCommandPool, nullptr);
			}

			ReleaseVkBuffer(m_vkGpuBuffer, m_vkGpuMemory);
		}

		void VlkStagingBuffer::SetData(const void *data) {

			memcpy(m_mappedDataP, data, m_size);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pCommandBuffers = &m_vkCommandBuffer;
			submitInfo.commandBufferCount = 1u;

			VlkDevice *vlkDeviceP = VlkRenderContext::GetVlkDevice();

			vkQueueSubmit(vlkDeviceP->GetActiveQueue().first, 1u, &submitInfo, m_vkFence);
			vkWaitForFences(vlkDeviceP->GetHandle(), 1u, &m_vkFence, true, 1000);
			vkResetFences(vlkDeviceP->GetHandle(), 1u, &m_vkFence);
		}

	}
}
