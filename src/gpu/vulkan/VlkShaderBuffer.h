#ifndef VLK_SHADER_BUFFER_H_
#define VLK_SHADER_BUFFER_H_

#include <ShaderBuffer.h>

#include <vulkan/vulkan.h>

namespace PixelMachine {
	namespace GPU {

		class VlkShaderBuffer : public ShaderBuffer {
		public:
			VlkShaderBuffer(const ShaderBufferType type, const ShaderBufferLayout dataLayout, const uint32_t elementCount);
			~VlkShaderBuffer();
			void SetData(const void *data) override;
			void Bind(const ShaderProgramType stage) const override;
			virtual uint32_t GetSize() const override { return m_size; };
		private:
			const uint32_t m_size = 0;
			void *m_mappedDataP = nullptr;
			VkBuffer m_vkStagingBuffer = VK_NULL_HANDLE;
			VkDeviceMemory m_vkStagingMemory = VK_NULL_HANDLE;
			VkBuffer m_vkDeviceLocalBuffer = VK_NULL_HANDLE;
			VkDeviceMemory m_vkDeviceLocalMemory = VK_NULL_HANDLE;
			
			VkCommandPool m_vkCommandPool = VK_NULL_HANDLE;
			VkCommandBuffer m_vkCommandBuffer = VK_NULL_HANDLE;
			VkFence m_vkFence = VK_NULL_HANDLE;
		};

	}
}

#endif // !VLK_SHADER_BUFFER_H_
