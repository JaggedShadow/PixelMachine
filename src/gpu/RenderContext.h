#ifndef RENDER_CONTEXT_H_
#define RENDER_CONTEXT_H_

#include <string>

namespace PixelMachine {
	namespace GPU {

		class RenderContext {
		public:
			static void Initialize(void *windowHandle);
			static RenderContext *Get();
			static void Destroy();

			virtual void BeginPass() = 0;
			virtual void SetPrimitiveType(const int type) = 0;
			virtual void SetLineWidth(const float width) = 0;
			virtual void SetMultisampling(const int sampleCount) = 0;
			virtual void SetDepthTesting(const bool enabled) = 0;
			virtual void SetClearColor(const float rgb[3]) = 0;
			virtual void SetViewport(const int xywh[4]) = 0;
			virtual void RunPass(const int index) = 0;
			virtual void PresentFrame() = 0;
			virtual void EndPass() = 0;

			virtual ~RenderContext() {};
		};
	}
}

#endif // !RENDER_CONTEXT_H_