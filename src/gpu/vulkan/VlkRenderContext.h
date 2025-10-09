#ifndef VLK_RENDER_CONTEXT_H_
#define VLK_RENDER_CONTEXT_H_

#include <RenderContext.h>

namespace PixelMachine {
	namespace GPU {
		class VlkDevice;
		class VlkRenderContext : public RenderContext {
		public:
			VlkRenderContext();
			~VlkRenderContext();
			void BeginFrame(const char *name) override {};
			void BindShader(const Shader &shader) override {};
			void BindVertexBuffer(const VertexBuffer &vbo) override {};
			void BindIndexBuffer(const IndexBuffer &ibo) override {};
			void BindUniformBuffer(const UniformBuffer &ubo) override {};
			void SetPrimitiveType(const int type) override {};
			void SetLineWidth(const float width) override {};
			void SetMultisampling(const int sampleCount) override {};
			void SetRenderTarget(void *windowHandle) override {};
			void SetRenderTarget(const TextureBuffer &texture) override {};
			void SetDepthTesting(const bool enabled) override {};
			void SetClearColor(const float rgb[3]) override {};
			void SetViewport(const int xywh[4]) override {};
			void Draw() override {};
			void PresentFrame() override {};
			static VlkDevice &GetVlkDevice(); // Reference getter added just for style (avoid access via public pointer)

		private:
			static VlkDevice *sm_vlkDeviceP;
		};
	}
}

#endif // !VLK_RENDER_CONTEXT_H_