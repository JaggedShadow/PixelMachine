#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>

#include <stdexcept>

namespace PixelMachine {
	namespace GPU {
		
		static VlkRenderContext *s_vlkRenderContextP;
		VlkDevice *VlkRenderContext::sm_vlkDeviceP = nullptr;

		void RenderContext::Initialize() {
			if (s_vlkRenderContextP == nullptr) {
				s_vlkRenderContextP = new VlkRenderContext();
			}
		}

		RenderContext *RenderContext::Get() {
			if (s_vlkRenderContextP == nullptr) {
				throw std::runtime_error("RenderContext not initialized.");
			}
			return s_vlkRenderContextP;
		}

		void RenderContext::Destroy() {
			if (s_vlkRenderContextP) {
				delete s_vlkRenderContextP;
			}
		}

		VlkRenderContext::VlkRenderContext() {
			if (sm_vlkDeviceP == nullptr) {
				sm_vlkDeviceP = new VlkDevice();
			}
		}

		VlkRenderContext::~VlkRenderContext() {
			delete sm_vlkDeviceP;
			sm_vlkDeviceP = nullptr;
		}

		VlkDevice &VlkRenderContext::GetVlkDevice() {
			return *sm_vlkDeviceP;
		}

	}
}