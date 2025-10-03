#include <vulkan/VlkRenderContext.h>

namespace PixelMachine {
	namespace GPU {
		
		static VlkRenderContext *g_vlkRenderContextP;

		void RenderContext::Allocate() {
			if (g_vlkRenderContextP == nullptr) {
				g_vlkRenderContextP = new VlkRenderContext();
			}
		}

		RenderContext *RenderContext::Get() {
			return g_vlkRenderContextP;
		}

		void RenderContext::Deallocate() {
			if (g_vlkRenderContextP) {
				delete g_vlkRenderContextP;
			}
		}

	}
}