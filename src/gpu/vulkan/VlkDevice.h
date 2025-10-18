#ifndef VLK_DEVICE_H_
#define VLK_DEVICE_H_

#include <vulkan/VlkAdapter.h>

#include <vector>
#include <optional>

namespace PixelMachine {
	namespace GPU {
		/// <summary>
		/// Main class that encapulates core Vulkan components
		/// required to interact with the API.
		/// </summary>
		class VlkDevice {
		public:
			enum QFExtraFlags {
				NONE = 0,
				WIN32_PRESENTATION = 1,
				OTHER = 1 << 1
			};
			VlkDevice();
			~VlkDevice();
			std::optional<uint32_t> GetQueueFamilyIndex(const uint32_t adapterIndex, VkQueueFlags queueFlags, QFExtraFlags extraFlags);
			bool SetAdapter(const uint32_t index);
			VlkAdapter GetAdapter(const uint32_t index) const;
			VlkAdapter GetActiveAdapter() const;
			uint32_t GetAdapterCount() const;
			VkDevice GetHandle() const;
			VkInstance GetVkInstance() const;

		private:
			VkInstance m_vkInstance = VK_NULL_HANDLE;
			VkDevice m_vkLogicalDevice = VK_NULL_HANDLE;
			std::vector<VlkAdapter> m_vlkAdapters;
			uint32_t m_activeAdapterIndex = 0u;
			// Graphics & presentation queue
			std::pair<VkQueue, uint32_t> m_vkGPQueue;

		};
	}
}


#endif // !VLK_DEVICE_H_