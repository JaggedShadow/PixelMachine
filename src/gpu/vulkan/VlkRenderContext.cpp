#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>
#include <vulkan/VlkSwapchain.h>

#include <stdexcept>

namespace PixelMachine {
	namespace GPU {
		
		extern VlkRenderContext *s_vlkRenderContextP;

		VlkRenderContext::VlkRenderContext(HWND windowHandle) {

			if (!sm_vlkDeviceP) {
				sm_vlkDeviceP = new VlkDevice();
			}

			VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
			surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceInfo.pNext = nullptr;
			surfaceInfo.hinstance = GetModuleHandle(NULL);
			surfaceInfo.hwnd = windowHandle;

			vkCreateWin32SurfaceKHR(sm_vlkDeviceP->GetVkInstance(), &surfaceInfo, nullptr, &m_vkSurface);

			if (!m_vkSurface) {
				throw new std::runtime_error("VlkRenderContext init fail - cannot create VkSurface.");
			}

			m_vkSurfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
			m_vkSurfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

			bool adapterNotFound = true;
			for (uint32_t i = 0; i < sm_vlkDeviceP->GetAdapterCount(); i++) {
				VlkAdapter adapter = sm_vlkDeviceP->GetAdapter(i);
				if (adapter.SurfaceFormatAvailable(m_vkSurface, m_vkSurfaceFormat) &&
					adapter.PresentModeAvailable(m_vkSurface, VK_PRESENT_MODE_FIFO_KHR)) {
					sm_vlkDeviceP->SetAdapter(i);
					adapterNotFound = false;
					break;
				}
			}

			if (adapterNotFound) {
				throw new std::runtime_error("VlkRenderContext init fail - cannot find suitable physical device.");
			}

			m_vlkSwapchainP = new VlkSwapchain(m_vkSurface, m_vkSurfaceFormat, VK_PRESENT_MODE_FIFO_KHR);

		}

		VlkRenderContext::~VlkRenderContext() {

			if (m_vlkSwapchainP) {
				delete m_vlkSwapchainP;
			}

			if (m_vkSurface) {
				vkDestroySurfaceKHR(sm_vlkDeviceP->GetVkInstance(), m_vkSurface, nullptr);
			}

			if (sm_vlkDeviceP) {
				delete sm_vlkDeviceP;
				sm_vlkDeviceP = nullptr;
			}
		}

	}
}