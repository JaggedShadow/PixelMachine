#include <vulkan/VlkAdapter.h>

#include <stdexcept>
#include <vector>

PixelMachine::GPU::VlkAdapter::VlkAdapter(VkPhysicalDevice physicalDevice) : m_vkPhysicalDevice(physicalDevice) {
	if (m_vkPhysicalDevice == VK_NULL_HANDLE) {
		throw new std::runtime_error("VlkAdapter constructor failed - null handle provided.");
	}
}

bool PixelMachine::GPU::VlkAdapter::PresentModeAvailable(const VkSurfaceKHR surface, const VkPresentModeKHR presentMode) const {
	
	uint32_t count = 0u;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, surface, &count, nullptr);

	std::vector<VkPresentModeKHR> presentModes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, surface, &count, presentModes.data());

	return std::find(presentModes.cbegin(), presentModes.cend(), presentMode) != presentModes.cend();
}

bool PixelMachine::GPU::VlkAdapter::SurfaceFormatAvailable(const VkSurfaceKHR surface, const VkSurfaceFormatKHR surfaceFormat) const {

	uint32_t count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, surface, &count, nullptr);

	std::vector<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, surface, &count, formats.data());

	bool formatFound = false;
	for (auto &format : formats) {
		if (format.format == surfaceFormat.format &&
			format.colorSpace == surfaceFormat.colorSpace) {
			return true;
		}
	}

	return false;
}

VkSurfaceCapabilitiesKHR PixelMachine::GPU::VlkAdapter::GetSurfaceInfo(const VkSurfaceKHR surface) const {
	
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, surface, &capabilities);

	return capabilities;
}

VkPhysicalDeviceMemoryProperties PixelMachine::GPU::VlkAdapter::GetMemoryInfo() const {
	
	VkPhysicalDeviceMemoryProperties properties = {};
	vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &properties);
	
	return properties;
}

VkPhysicalDevice PixelMachine::GPU::VlkAdapter::GetHandle() const {
	return m_vkPhysicalDevice;
}
