#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>
#include <vulkan/VlkSwapchain.h>
#include <vulkan/VlkShaderProgram.h>

#include <stdexcept>

namespace PixelMachine {
	namespace GPU {

		VlkRenderContext *s_vlkRenderContextP;
		VlkDevice *VlkRenderContext::sm_vlkDeviceP = nullptr;

		ShaderProgram *ShaderProgram::CreateFromCompiled(const std::string name, const std::string compiledShaderPath, ShaderType type) {
			return new VlkShaderProgram(name, compiledShaderPath, type);
		}

		void RenderContext::Initialize(void *windowHandle) {
			if (!s_vlkRenderContextP) {
				s_vlkRenderContextP = new VlkRenderContext(static_cast<HWND>(windowHandle));
			}
		}

		RenderContext *RenderContext::Get() {
			if (!s_vlkRenderContextP) {
				throw new std::runtime_error("VlkRenderContext access fail - not initialized.");
			}
			return s_vlkRenderContextP;
		}

		void RenderContext::Destroy() {
			if (s_vlkRenderContextP) {
				delete s_vlkRenderContextP;
			}
		}

		VlkDevice *VlkRenderContext::GetVlkDevice() {
			if (!sm_vlkDeviceP) {
				throw new std::runtime_error("VlkDevice access failed - not initialized.");
			}
			return sm_vlkDeviceP;
		}
	}
 }