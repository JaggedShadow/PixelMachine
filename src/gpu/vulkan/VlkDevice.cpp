#include <vulkan/VlkDevice.h>
#include <stdexcept>

static VkInstance CreateVkInstance() {
	
	VkApplicationInfo vkAppInfo = {};
	vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	vkAppInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	vkAppInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &vkAppInfo;

	const char khrSurfaceExtName[] = VK_KHR_SURFACE_EXTENSION_NAME;
	const char khrWin32SurfaceExtName[] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
	const char *extensionsNames[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
	instanceInfo.ppEnabledExtensionNames = extensionsNames;
	instanceInfo.enabledExtensionCount = 2u;
	instanceInfo.enabledLayerCount = 0u;

#ifdef VK_ENABLE_VALIDATION
	const char *validationLayerName = "VK_LAYER_KHRONOS_validation";
	instanceInfo.enabledLayerCount = 1u;
	instanceInfo.ppEnabledLayerNames = &validationLayerName;
#endif

	VkInstance result = VK_NULL_HANDLE;
	vkCreateInstance(&instanceInfo, nullptr, &result);

	return result;
}

PixelMachine::GPU::VlkDevice::VlkDevice() {
	m_vkInstance = CreateVkInstance();
	if (m_vkInstance == VK_NULL_HANDLE) {
		throw std::runtime_error("VlkDevice constructor failed - unable to create VkInstance.");
	}

	uint32_t physicalDevicesCount = 0u;
	vkEnumeratePhysicalDevices(m_vkInstance, &physicalDevicesCount, nullptr);

	if (!physicalDevicesCount) {
		throw std::runtime_error("VlkDevice constructor failed - No GPUs detected.");
	}

	m_vkPhysicalDevices.resize(physicalDevicesCount);
	vkEnumeratePhysicalDevices(m_vkInstance, &physicalDevicesCount, m_vkPhysicalDevices.data());

	auto qfIndex = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT, QFExtraFlags::WIN32_PRESENTATION);

	if (!qfIndex.has_value()) {
		throw std::runtime_error("VlkDevice constructor failed - Required VkQueue family not found.");
	}
}

PixelMachine::GPU::VlkDevice::~VlkDevice() {
	if (m_vkInstance) {
		vkDestroyInstance(m_vkInstance, nullptr);
	}
}

std::optional<uint32_t> PixelMachine::GPU::VlkDevice::GetQueueFamilyIndex(VkQueueFlags queueFlags, QFExtraFlags extraFlags) {

	uint32_t familiesCount = 0u;
	vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevices[m_deviceIndex], &familiesCount, nullptr);

	if (!familiesCount) {
		return std::nullopt;
	}

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(familiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevices[m_deviceIndex], &familiesCount, queueFamilyProperties.data());

	uint32_t i = 0;
	for (auto &properties : queueFamilyProperties) {
		if (properties.queueFlags & queueFlags) {
			if (extraFlags & QFExtraFlags::WIN32_PRESENTATION) {
				if (!vkGetPhysicalDeviceWin32PresentationSupportKHR(m_vkPhysicalDevices[m_deviceIndex], i)) {
					return std::nullopt;
				}
			}
			return i;
		}
		i++;
	}

	return std::nullopt;
}
