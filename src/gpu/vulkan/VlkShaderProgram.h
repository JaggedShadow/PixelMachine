#ifndef VLK_SHADER_PROGRAM_H_
#define VLK_SHADER_PROGRAM_H_

#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>
#include <vulkan/vulkan.h>

#include <fstream>

namespace PixelMachine {
	namespace GPU {

		class VlkShaderProgram : public ShaderProgram {
		public:
			VlkShaderProgram(const std::string name, const std::string compiledShaderPath, ShaderType type) : ShaderProgram(name, type, 0) {
				
				std::ifstream file(compiledShaderPath, std::ios::ate | std::ios::binary);

				if (!file.is_open()) {
					throw std::runtime_error("VlkShaderProgram construction fail - file not found.");
				}

				size_t fileSize = static_cast<size_t>(file.tellg());
				std::vector<char> buffer(fileSize);

				file.seekg(0);
				file.read(buffer.data(), fileSize);
				file.close();

				VkShaderModuleCreateInfo shaderModuleCI = {};
				shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				shaderModuleCI.codeSize = buffer.size();
				shaderModuleCI.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

				VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();
				vkCreateShaderModule(device, &shaderModuleCI, nullptr, &m_vkShaderModule);

			};

			~VlkShaderProgram() {
				if (m_vkShaderModule) {
					VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();
					vkDestroyShaderModule(device, m_vkShaderModule, nullptr);
				}
			};

			VkShaderModule GetVkShaderModule() const { return m_vkShaderModule; }

			void Bind() const override {

				VlkRenderContext *pVlkRenderContext = static_cast<VlkRenderContext*>(VlkRenderContext::Get());

				switch (m_type)
				{
				case PixelMachine::GPU::VertexShader:
					//pVlkRenderContext->BindVertexShader(m_vkShaderModule);
					break;
				case PixelMachine::GPU::FragmentShader:
					//pVlkRenderContext->BindFragmentShader(m_vkShaderModule);
					break;
				case PixelMachine::GPU::ComputeShader:
					//pVlkRenderContext->BindComputeShader(m_vkShaderModule);
					break;
				default:
					break;
				}
			};

		private:
			VkShaderModule m_vkShaderModule = VK_NULL_HANDLE;
		};

	}
}


#endif // !VLK_SHADER_PROGRAM_H_
