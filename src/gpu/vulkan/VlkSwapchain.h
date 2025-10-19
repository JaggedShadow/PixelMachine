#ifndef VLK_SWAPCHAIN_H_
#define VLK_SWAPCHAIN_H_

#include <vulkan/vulkan.h>

#include <vector>

namespace PixelMachine {
	namespace GPU {
		class VlkSwapchain {
		public:
			VlkSwapchain() {};
			VlkSwapchain(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode);
			~VlkSwapchain();
			VkRenderPass GetVkRenderPass() const { return m_vkRenderPass; }
			uint32_t GetImage(VkFramebuffer *outFramebuffer) const;
			VkSemaphore GetImageAvailableSemaphore() const { return m_vkImageAvailableSemaphore; }

		private:
			VkSurfaceFormatKHR m_vkSurfaceFormat = {};
			VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
			VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
			VkSemaphore m_vkImageAvailableSemaphore = VK_NULL_HANDLE;
			std::vector<VkImageView> m_frameViews;
			std::vector<VkFramebuffer> m_framebuffers;

		};
	}
}


#endif // !VLK_SWAPCHAIN_H_
