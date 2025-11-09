#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "ShaderEnum.h"

#include <string>

namespace PixelMachine {
	namespace GPU {

		class ShaderProgram {
		public:
			static ShaderProgram *CreateFromCompiled(const std::string name, const std::string compiledShaderPath, ShaderProgramType type);
			ShaderProgramType GetType() const { return m_type; }
			virtual void Bind() const = 0;
			virtual ~ShaderProgram() {};
		protected:
			ShaderProgram(std::string name, ShaderProgramType shaderType, size_t size) : m_name(name), m_type(shaderType), m_size(size) {};
			std::string m_name;
			ShaderProgramType m_type;
			size_t m_size;
		};
	}
}

#endif // !SHADER_PROGRAM_H_
