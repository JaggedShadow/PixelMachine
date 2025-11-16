#ifndef SHADER_BUFFER_H_
#define SHADER_BUFFER_H_

#include "ShaderEnum.h"

#include <string>
#include <vector>

namespace PixelMachine {
	namespace GPU {

		enum BufferType {
			VertexBuffer,
			IndexBuffer,
			UniformBuffer
		};

		enum BufferDataType {
			int2,
			int3,
			int4,
			uint2,
			uint3,
			uint4,
			float2,
			float3,
			float4,
			matrix3,
			matrix4
		};

		struct BufferAttribute {
		public:
			BufferAttribute(BufferDataType shaderDataType, const std::string name) :
				m_name(name),
				m_shaderDataType(shaderDataType),
				m_size(GetSize(m_shaderDataType)),
				m_offset(0) {}

			const std::string m_name;
			const BufferDataType m_shaderDataType;
			const uint32_t m_size;
			uint32_t m_offset;

		private:
			static uint32_t GetSize(BufferDataType dataType) {
				switch (dataType)
				{
				case BufferDataType::int3:		return 4 * 3;
				case BufferDataType::int4:		return 4 * 4;
				case BufferDataType::uint3:		return 4 * 3;
				case BufferDataType::uint4:		return 4 * 4;
				case BufferDataType::float3:	return 4 * 3;
				case BufferDataType::float4:	return 4 * 4;
				case BufferDataType::matrix3:	return 4 * 3 * 3;
				case BufferDataType::matrix4:	return 4 * 4 * 4;
				default: break;
				}
				return 0;
			}
		};

		class BufferLayout {
		public:
			BufferLayout(std::initializer_list<BufferAttribute> bufferAttributes)
				: m_attributes(bufferAttributes.begin(), bufferAttributes.end()) {
				uint32_t offset = 0;
				for (auto &attribute : m_attributes) {
					attribute.m_offset = offset;
					offset += attribute.m_size;
				}
				m_size = offset;
			};
			uint32_t GetAttributeCount() const { return m_attributes.size(); };
			uint32_t GetSize() const { return m_size; };
			BufferAttribute GetAttribute(uint32_t index) const { return m_attributes[index]; };
		private:
			std::vector<BufferAttribute> m_attributes;
			uint32_t m_size = 0;
		};


		class Buffer {
		public:
			/* Creates a buffer object
			 <bindStage> parameter is used only if <type> is BufferType::Uniform */
			static Buffer *Create(
				const BufferType type,
				const ShaderProgramType bindStage,
				const BufferLayout dataLayout,
				const uint32_t elementCount);
			ShaderProgramType GetBindStage() const { return m_bindStage; };
			BufferType GetType() const { return m_type; }
			BufferLayout GetLayout() const { return m_dataLayout; }
			virtual void Bind() const = 0;
			virtual void SetData(const void *data) = 0;
			virtual uint32_t GetSize() const = 0;
			virtual ~Buffer() {};
		protected:
			Buffer(
				const BufferType type,
				const ShaderProgramType bindStage,
				const BufferLayout dataLayout)
				: m_type(type), m_bindStage(bindStage), m_dataLayout(dataLayout) {

				if (m_type == BufferType::VertexBuffer || m_type == BufferType::IndexBuffer) {
					m_bindStage = ShaderProgramType::VertexShader;
				}
			};
			BufferType m_type;
			ShaderProgramType m_bindStage;
			BufferLayout m_dataLayout;
		};
	}
}

#endif // !SHADER_BUFFER_H_
