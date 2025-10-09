#ifndef VLK_ADAPTER_H_
#define VLK_ADAPTER_H_

#include <vulkan/vulkan.h>

namespace PixelMachine {
	namespace GPU {
		/// <summary>
		/// Wrapper class for VkLogicalDevice. Provides some basic, but
		/// useful information about particular video adapter.
		/// </summary>
		class VlkAdapter {
		public:
			VlkAdapter(VkPhysicalDevice physicalDevice);
			bool PresentModeAvailable(const VkSurfaceKHR surface, const VkPresentModeKHR presentMode) const;
			bool SurfaceFormatAvailable(const VkSurfaceKHR surface, const VkSurfaceFormatKHR surfaceFormat) const;
			VkSurfaceCapabilitiesKHR GetSurfaceInfo(const VkSurfaceKHR surface) const;
			VkPhysicalDeviceMemoryProperties GetMemoryInfo() const;
			VkPhysicalDevice GetHandle() const;

		private:
			VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
		};
	}
}

#endif // VLK_ADAPTER_H_