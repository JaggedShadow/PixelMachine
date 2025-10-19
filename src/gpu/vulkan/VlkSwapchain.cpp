#include <vulkan/VlkSwapchain.h>
#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>

#include <stdexcept>

static VkRenderPass CreateVkRenderPass(VkFormat imageFormat) {

	VkAttachmentDescription attchDesc = {};
	attchDesc.format = imageFormat;
	attchDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	attchDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attchDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attchDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attchDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attchDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attchDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attchRef = {};
	attchRef.attachment = 0;
	attchRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments = &attchRef;

	VkSubpassDependency subpassDep = {};
	subpassDep.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDep.dstSubpass = 0;
	subpassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDep.srcAccessMask = 0;
	subpassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &attchDesc;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDesc;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &subpassDep;

	auto device = PixelMachine::GPU::VlkRenderContext::GetVlkDevice()->GetHandle();

	VkRenderPass result = VK_NULL_HANDLE;
	vkCreateRenderPass(device, &renderPassInfo, nullptr, &result);

	return result;
}

static VkSwapchainKHR CreateVkSwapchain(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode) {

	VkSwapchainCreateInfoKHR swapchainInfo = {};
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.pNext = nullptr;
	swapchainInfo.flags = NULL;
	swapchainInfo.surface = surface;

	PixelMachine::GPU::VlkDevice *device = PixelMachine::GPU::VlkRenderContext::GetVlkDevice();
	VkSurfaceCapabilitiesKHR capabilities = device->GetActiveAdapter().GetSurfaceInfo(surface);

	swapchainInfo.minImageCount = capabilities.minImageCount + 1;
	swapchainInfo.imageArrayLayers = capabilities.maxImageArrayLayers;
	swapchainInfo.imageExtent.width = capabilities.currentExtent.width;
	swapchainInfo.imageExtent.height = capabilities.currentExtent.height;

	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;

	swapchainInfo.presentMode = presentMode;

	swapchainInfo.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount = 0;
	swapchainInfo.pQueueFamilyIndices = nullptr;
	swapchainInfo.preTransform = capabilities.currentTransform;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR result = VK_NULL_HANDLE;
	vkCreateSwapchainKHR(device->GetHandle(), &swapchainInfo, nullptr, &result);

	return result;
}

PixelMachine::GPU::VlkSwapchain::VlkSwapchain(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode)
	: m_vkSurfaceFormat(surfaceFormat),
	m_vkRenderPass(CreateVkRenderPass(surfaceFormat.format)),
	m_vkSwapchain(CreateVkSwapchain(surface, surfaceFormat, presentMode)) {

	if (!m_vkRenderPass) {
		throw new std::runtime_error("VlkSwapchain constructor failed - unable to create a render pass.");
	}

	if (!m_vkSwapchain) {
		throw new std::runtime_error("VlkSwapchain constructor failed - unable to create a swapchain.");
	}


}

PixelMachine::GPU::VlkSwapchain::~VlkSwapchain() {

	VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();

	if (m_vkSwapchain) {
		vkDestroySwapchainKHR(device, m_vkSwapchain, nullptr);
	}

	if (m_vkRenderPass) {
		vkDestroyRenderPass(device, m_vkRenderPass, nullptr);
	}

}
