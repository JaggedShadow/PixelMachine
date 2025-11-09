#ifndef SHADER_ENUM_H_
#define SHADER_ENUM_H_

namespace PixelMachine {
	namespace GPU {

		enum ShaderProgramType {
			Undefined,
			VertexShader,
			FragmentShader,
			ComputeShader
		};

		enum ShaderBufferType {
			VertexBuffer,
			IndexBuffer,
			UniformBuffer
		};

		enum ShaderDataType {
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
	}
}

#endif // !SHADER_ENUM_H_
