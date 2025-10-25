#ifndef VLK_RENDER_CONTEXT_H_
#define VLK_RENDER_CONTEXT_H_

#include <RenderContext.h>

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace PixelMachine {
	namespace GPU {
		class VlkDevice;
		class VlkSwapchain;
		class VlkRenderContext : public RenderContext {
		public:
			VlkRenderContext(HWND windowHandle);
			~VlkRenderContext();
			void BeginPass(const std::string name) override {};
			void SetPrimitiveType(const int type) override {};
			void SetLineWidth(const float width) override {};
			void SetMultisampling(const int sampleCount) override {};
			void SetDepthTesting(const bool enabled) override {};
			void SetClearColor(const float rgb[3]) override {};
			void SetViewport(const int xywh[4]) override {};
			void RunPass(const std::string name) override {};
			void PresentFrame() override {};
			void EndPass() override {};
			static VlkDevice *GetVlkDevice();

		private:
			static VlkDevice *sm_vlkDeviceP;
			VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
			VkSurfaceFormatKHR m_vkSurfaceFormat = {};
			VlkSwapchain *m_vlkSwapchainP = nullptr;

		};
	}
}

#endif // !VLK_RENDER_CONTEXT_H_