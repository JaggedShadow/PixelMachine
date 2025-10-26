#ifndef RENDER_CONTEXT_H_
#define RENDER_CONTEXT_H_

#include <string>

namespace PixelMachine {
	namespace GPU {

		enum ShaderType {
			VertexShader,
			FragmentShader,
			ComputeShader
		};

		class ShaderProgram {
		public:
			static ShaderProgram *CreateFromCompiled(const std::string name, const std::string compiledShaderPath, ShaderType type);
			ShaderType GetType() const { return m_type; }
			virtual void Bind() const = 0;
			virtual ~ShaderProgram() {};
		protected:
			ShaderProgram(std::string name, ShaderType shaderType, size_t size) : m_name(name), m_type(shaderType), m_size(size) {};
			std::string m_name;
			ShaderType m_type;
			size_t m_size;
		};

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