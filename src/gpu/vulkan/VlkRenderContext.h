#ifndef VLK_RENDER_CONTEXT_H_
#define VLK_RENDER_CONTEXT_H_

#include <RenderContext.h>

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace PixelMachine {
	namespace GPU {	
		class VlkShaderProgram;
		class VlkDevice;
		class VlkSwapchain;
		class VlkRenderContext : public RenderContext {
		public:
			VlkRenderContext(HWND windowHandle);
			~VlkRenderContext();
			void BeginPass() override { m_vlkPasses.push_back(VlkPass()); };
			void SetPrimitiveType(const int type) override {};
			void SetLineWidth(const float width) override {};
			void SetMultisampling(const int sampleCount) override {};
			void SetDepthTesting(const bool enabled) override {};
			void SetClearColor(const float rgb[3]) override {};
			void SetViewport(const int xywh[4]) override {};
			void RunPass(const int index) override {};
			void PresentFrame() override {};
			void EndPass() override;
			static VlkDevice *GetVlkDevice();

			void BindShaderProgram(const VlkShaderProgram *shaderProgram);

		private:

			struct VlkPass {
				VkPipeline m_vkPipeline = VK_NULL_HANDLE;
				VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
				std::vector<VkPipelineShaderStageCreateInfo> m_shaderStagesInfo;
				bool m_renderToScreen = true;
				bool m_depthTest = false;
				float m_lineWidth = 0.5;
				uint32_t m_primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				uint32_t m_msaaSamples = 1u;
				float m_clearColor[3] = { 0 };
				float m_viewportRect[4] = { 0 };

				~VlkPass();
			};

			static VlkDevice *sm_vlkDeviceP;
			VkSurfaceKHR m_vkWinSurface = VK_NULL_HANDLE;
			VkSurfaceFormatKHR m_vkWinSurfaceFormat = {};
			VlkSwapchain *m_vlkSwapchainP = nullptr;
			std::vector<VlkPass> m_vlkPasses;
		};
	}
}

#endif // !VLK_RENDER_CONTEXT_H_