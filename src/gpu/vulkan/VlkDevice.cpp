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

static VkDevice CreateVkDevice(VkPhysicalDevice physicalDevice, const uint32_t qfIndex) {

	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = qfIndex;
	queueInfo.queueCount = 1u;
	float priority = 1.f;
	queueInfo.pQueuePriorities = &priority;

	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.queueCreateInfoCount = 1;

	VkPhysicalDeviceFeatures features = {};
	deviceInfo.pEnabledFeatures = &features;

	std::vector<const char*> extensions = { 
		VK_KHR_SWAPCHAIN_EXTENSION_NAME ,
		VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
		VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME
	};

	deviceInfo.ppEnabledExtensionNames = extensions.data();
	deviceInfo.enabledExtensionCount = extensions.size();

	VkDevice vkDevice = VK_NULL_HANDLE;
	vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &vkDevice);

	return vkDevice;
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

	std::vector<VkPhysicalDevice> vkPhysicalDevices;

	vkPhysicalDevices.resize(physicalDevicesCount);
	vkEnumeratePhysicalDevices(m_vkInstance, &physicalDevicesCount, vkPhysicalDevices.data());

	for (auto &pd : vkPhysicalDevices) {
		m_vlkAdapters.push_back(VlkAdapter(pd));
	}
}

PixelMachine::GPU::VlkDevice::~VlkDevice() {

	if (m_vkLogicalDevice) {
		vkDestroyDevice(m_vkLogicalDevice, nullptr);
	}

	if (m_vkInstance) {
		vkDestroyInstance(m_vkInstance, nullptr);
	}
}

std::optional<uint32_t> PixelMachine::GPU::VlkDevice::GetQueueFamilyIndex(const uint32_t adapterIndex, VkQueueFlags queueFlags, QFExtraFlags extraFlags) {
	
	if (adapterIndex < 0 || adapterIndex >= m_vlkAdapters.size()) {
		return std::nullopt;
	}

	uint32_t familiesCount = 0u;
	vkGetPhysicalDeviceQueueFamilyProperties(m_vlkAdapters[adapterIndex].GetHandle(), &familiesCount, nullptr);

	if (!familiesCount) {
		return std::nullopt;
	}

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(familiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_vlkAdapters[adapterIndex].GetHandle(), &familiesCount, queueFamilyProperties.data());

	uint32_t i = 0;
	for (auto &properties : queueFamilyProperties) {
		if (properties.queueFlags & queueFlags) {
			if (extraFlags & QFExtraFlags::WIN32_PRESENTATION) {
				if (!vkGetPhysicalDeviceWin32PresentationSupportKHR(m_vlkAdapters[adapterIndex].GetHandle(), i)) {
					return std::nullopt;
				}
			}
			return i;
		}
		i++;
	}

	return std::nullopt;
}

bool PixelMachine::GPU::VlkDevice::SetAdapter(const uint32_t index) {

	if (index < 0 || index >= m_vlkAdapters.size()) {
		return false;
	}

	auto qfIndex = GetQueueFamilyIndex(index ,VK_QUEUE_GRAPHICS_BIT, QFExtraFlags::WIN32_PRESENTATION);

	if (!qfIndex.has_value()) {
		return false;
	}

	VkDevice newLogicalDevice = VK_NULL_HANDLE;
	newLogicalDevice = CreateVkDevice(GetAdapter(index).GetHandle(), qfIndex.value());

	if (!newLogicalDevice) {
		return false;
	}

	if (m_vkLogicalDevice) {
		vkDestroyDevice(m_vkLogicalDevice, nullptr);
	}

	m_activeAdapterIndex = index;
	m_vkLogicalDevice = newLogicalDevice;
	m_vkGPQueue.second = qfIndex.value();
	vkGetDeviceQueue(m_vkLogicalDevice, qfIndex.value(), 0, &(m_vkGPQueue.first));
	
	return true;
}

uint32_t PixelMachine::GPU::VlkDevice::GetAdapterCount() const {
	return m_vlkAdapters.size();
}

PixelMachine::GPU::VlkAdapter PixelMachine::GPU::VlkDevice::GetActiveAdapter() const {
	return m_vlkAdapters[m_activeAdapterIndex];
}

PixelMachine::GPU::VlkAdapter PixelMachine::GPU::VlkDevice::GetAdapter(const uint32_t index) const {
	if (index < 0 || index >= m_vlkAdapters.size()) {
		throw new std::runtime_error("VlkAdapter access fail - index out of range.");
	}
	return m_vlkAdapters[index];
}

VkDevice PixelMachine::GPU::VlkDevice::GetHandle() const {
	return m_vkLogicalDevice;
}

VkInstance PixelMachine::GPU::VlkDevice::GetVkInstance() const {
	return m_vkInstance;
}
