#include <vulkan/VlkRenderContext.h>

namespace PixelMachine {
	namespace GPU {
		
		static VlkRenderContext *s_vlkRenderContextP;

		void RenderContext::Initialize() {
			if (s_vlkRenderContextP == nullptr) {
				s_vlkRenderContextP = new VlkRenderContext();
			}
		}

		RenderContext *RenderContext::Get() {
			return s_vlkRenderContextP;
		}

		void RenderContext::Destroy() {
			if (s_vlkRenderContextP) {
				delete s_vlkRenderContextP;
			}
		}

	}
}