#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>

#include <stdexcept>

namespace PixelMachine {
	namespace GPU {
		
		static VlkRenderContext *s_vlkRenderContextP;
		VlkDevice *VlkRenderContext::sm_vlkDeviceP = nullptr;

		void RenderContext::Initialize() {
			if (!s_vlkRenderContextP) {
				s_vlkRenderContextP = new VlkRenderContext();
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

		VlkRenderContext::VlkRenderContext() {
			if (!sm_vlkDeviceP) {
				sm_vlkDeviceP = new VlkDevice();
				if (!sm_vlkDeviceP->SetAdapter(0)) {
					throw new std::runtime_error("VlkAdapter setup fail.");
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