#ifndef SHADER_BUFFER_H_
#define SHADER_BUFFER_H_

#include "ShaderEnum.h"

#include <string>
#include <vector>

namespace PixelMachine {
	namespace GPU {

		struct ShaderBufferAttribute {
		public:
			ShaderBufferAttribute(ShaderDataType shaderDataType, const std::string name) :
				m_name(name),
				m_shaderDataType(shaderDataType),
				m_size(GetSize(m_shaderDataType)),
				m_offset(0) {}

			const std::string m_name;
			const ShaderDataType m_shaderDataType;
			const uint32_t m_size;
			uint32_t m_offset;

		private:
			static uint32_t GetSize(ShaderDataType dataType) {
				switch (dataType)
				{
				case ShaderDataType::int3:		return 4 * 3;
				case ShaderDataType::int4:		return 4 * 4;
				case ShaderDataType::uint3:		return 4 * 3;
				case ShaderDataType::uint4:		return 4 * 4;
				case ShaderDataType::float3:	return 4 * 3;
				case ShaderDataType::float4:	return 4 * 4;
				case ShaderDataType::matrix3:	return 4 * 3 * 3;
				case ShaderDataType::matrix4:	return 4 * 4 * 4;
				default: break;
				}
				return 0;
			}
		};

		class ShaderBufferLayout {
		public:
			ShaderBufferLayout(std::initializer_list<ShaderBufferAttribute> bufferAttributes) {
				uint32_t offset = 0;
				for (auto &attribute : m_attributes) {
					attribute.m_offset = offset;
					offset += attribute.m_size;
				}
				m_size = offset;
			};
			uint32_t GetAttributeCount() const { return m_attributes.size(); };
			uint32_t GetSize() const { return m_size; };
			ShaderBufferAttribute GetAttribute(uint32_t index) const { return m_attributes[index]; };
		private:
			std::vector<ShaderBufferAttribute> m_attributes;
			uint32_t m_size = 0;
		};


		class ShaderBuffer {
		public:
			static ShaderBuffer *Create(const ShaderBufferLayout dataLayout, const void *data, const ShaderBufferType type);
			/// <summary>
			/// Binds a buffer to the pipeline
			/// </summary>
			/// <param name="stage"> Pipeline stage the buffer is about to be bound
			/// - ignored if buffer type is other than "Uniform" </param>
			virtual void Bind(const ShaderProgramType stage = ShaderProgramType::Undefined) const = 0;
		protected:
			ShaderBuffer(const ShaderBufferLayout dataLayout, const ShaderBufferType type) : m_dataLayout(dataLayout), m_type(type) {};
			virtual ~ShaderBuffer() {};
			ShaderBufferType m_type;
			ShaderBufferLayout m_dataLayout;
		};
	}
}

#endif // !SHADER_BUFFER_H_
