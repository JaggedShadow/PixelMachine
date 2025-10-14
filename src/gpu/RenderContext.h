#ifndef RENDER_CONTEXT_H_
#define RENDER_CONTEXT_H_

namespace PixelMachine {
	namespace GPU {

		class Shader;
		class VertexBuffer;
		class IndexBuffer;
		class UniformBuffer;
		class TextureBuffer;

		class RenderContext {
		public:
			static void Initialize(void *windowHandle);
			static RenderContext *Get();
			static void Destroy();

			virtual void BeginPass(const char* name) = 0;
			virtual void BindShader(const Shader& shader) = 0;
			virtual void BindVertexBuffer(const VertexBuffer& vbo) = 0;
			virtual void BindIndexBuffer(const IndexBuffer& ibo) = 0;
			virtual void BindUniformBuffer(const UniformBuffer& ubo) = 0;
			virtual void SetPrimitiveType(const int type) = 0;
			virtual void SetLineWidth(const float width) = 0;
			virtual void SetMultisampling(const int sampleCount) = 0;
			virtual void SetRenderTarget(const TextureBuffer& texture) = 0;
			virtual void SetDepthTesting(const bool enabled) = 0;
			virtual void SetClearColor(const float rgb[3]) = 0;
			virtual void SetViewport(const int xywh[4]) = 0;
			virtual void Draw() = 0;
			virtual void PresentFrame() = 0;
			virtual void EndPass() = 0;

			virtual ~RenderContext() {};
		};
	}
}

#endif // !RENDER_CONTEXT_H_