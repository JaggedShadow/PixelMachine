#include <vulkan/VulkanContext.h>

std::string PixelMachine::GPU::VulkanContext::GetInfo() const { return "VulkanContext initialized"; }

PixelMachine::GPU::Context * PixelMachine::GPU::Context::Get(void *windowHandle) {
	return new VulkanContext();
}