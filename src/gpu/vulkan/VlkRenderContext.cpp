#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>

#include <stdexcept>

namespace PixelMachine {
	namespace GPU {
		
		static VlkRenderContext *s_vlkRenderContextP;
		VlkDevice *VlkRenderContext::sm_vlkDeviceP = nullptr;

		void RenderContext::Initialize(void *windowHandle) {
			if (!s_vlkRenderContextP) {
				s_vlkRenderContextP = new VlkRenderContext(static_cast<HWND>(windowHandle));
			}
		}

		RenderContext *RenderContext::Get() {
			if (!s_vlkRenderContextP) {
				throw new std::runtime_error("VlkRenderContext access fail - not initialized.");
			}
			return s_vlkRenderContextP;
		}

		void RenderContext::Destroy() {
			if (s_vlkRenderContextP) {
				delete s_vlkRenderContextP;
			}
		}

		VlkRenderContext::VlkRenderContext(HWND windowHandle) {

			if (!sm_vlkDeviceP) {
				sm_vlkDeviceP = new VlkDevice();
			}

			m_vkSurface = sm_vlkDeviceP->CreateWindowSurface(windowHandle);

			if (!m_vkSurface) {
				throw new std::runtime_error("VlkRenderContext init fail - cannot create VkSurface.");
			}

			m_vkSurfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
			m_vkSurfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

			for (uint32_t i = 0; i < sm_vlkDeviceP->GetAdapterCount(); i++) {
				VlkAdapter adapter = sm_vlkDeviceP->GetAdapter(i);
				if (adapter.SurfaceFormatAvailable(m_vkSurface, m_vkSurfaceFormat) &&
					adapter.PresentModeAvailable(m_vkSurface, VK_PRESENT_MODE_FIFO_KHR)) {
					sm_vlkDeviceP->SetAdapter(i);
					break;
				}
			}
		}

		VlkRenderContext::~VlkRenderContext() {
			if (sm_vlkDeviceP) {
				delete sm_vlkDeviceP;
				sm_vlkDeviceP = nullptr;
			}
		}

		VlkDevice &VlkRenderContext::GetVlkDevice() {
			if (!sm_vlkDeviceP) {
				throw new std::runtime_error("VlkDevice access failed - not initialized.");
			}
			return *sm_vlkDeviceP;
		}
	}
}