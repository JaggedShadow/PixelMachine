#ifndef VLK_DEVICE_H_
#define VLK_DEVICE_H_

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

namespace PixelMachine {
	namespace GPU {
		class VlkDevice {
		public:
			enum QFExtraFlags {
				NONE = 0,
				WIN32_PRESENTATION = 1,
				OTHER = 1 << 1
			};
			VlkDevice();
			~VlkDevice();
			std::optional<uint32_t> GetQueueFamilyIndex(VkQueueFlags queueFlags, QFExtraFlags extraFlags);
		private:
			VkInstance m_vkInstance = VK_NULL_HANDLE;
			VkDevice m_vkLogicalDevice = VK_NULL_HANDLE;
			std::vector<VkPhysicalDevice> m_vkPhysicalDevices;
			uint32_t m_deviceIndex = 0u;
			std::pair<VkQueue, uint32_t> m_vkGraphicsPresentQueue;

		};
	}
}


#endif // !VLK_DEVICE_H_