#ifndef VLK_SHADER_BUFFER_H_
#define VLK_SHADER_BUFFER_H_

#include <Buffer.h>

#include <vulkan/vulkan.h>

namespace PixelMachine {
	namespace GPU {

		class VlkBuffer : public Buffer {
		public:
			VlkBuffer(
				const BufferType type,
				const ShaderProgramType bindStage, 
				const BufferLayout dataLayout,
				const uint32_t elementCount);
			VlkBuffer(
				const BufferType type,
				const ShaderProgramType bindStage,
				const BufferLayout dataLayout);
			virtual ~VlkBuffer();
			virtual void SetData(const void *data) override;
			void Bind() const override;
			uint32_t GetSize() const override { return m_size; };
			virtual VkBuffer GetHandle() const { return m_vkHostBuffer; };
		protected:
			uint32_t m_size = 0;
			void *m_mappedDataP = nullptr;
			VkBuffer m_vkHostBuffer = VK_NULL_HANDLE;
			VkDeviceMemory m_vkHostMemory = VK_NULL_HANDLE;
		};

		class VlkStagingBuffer : public VlkBuffer {
		public:
			VlkStagingBuffer(
				const BufferType type,
				const ShaderProgramType bindStage,
				const BufferLayout dataLayout,
				const uint32_t elementCount);
			~VlkStagingBuffer();
			void SetData(const void *data) override;
			VkBuffer GetHandle() const { return m_vkGpuBuffer; };
		private:
			VkBuffer m_vkGpuBuffer = VK_NULL_HANDLE;
			VkDeviceMemory m_vkGpuMemory = VK_NULL_HANDLE;
			VkCommandPool m_vkCommandPool = VK_NULL_HANDLE;
			VkCommandBuffer m_vkCommandBuffer = VK_NULL_HANDLE;
			VkFence m_vkFence = VK_NULL_HANDLE;
		};
	}
}

#endif // !VLK_SHADER_BUFFER_H_