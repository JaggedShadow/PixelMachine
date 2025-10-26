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
	VkSurfaceCapabilitiesKHR caps = device->GetActiveAdapter().GetSurfaceInfo(surface);

	swapchainInfo.minImageCount = caps.minImageCount + 1;
	swapchainInfo.imageArrayLayers = caps.maxImageArrayLayers;
	swapchainInfo.imageExtent.width = caps.currentExtent.width;
	swapchainInfo.imageExtent.height = caps.currentExtent.height;

	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;

	swapchainInfo.presentMode = presentMode;

	swapchainInfo.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount = 0;
	swapchainInfo.pQueueFamilyIndices = nullptr;
	swapchainInfo.preTransform = caps.currentTransform;
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

	VlkDevice *device = VlkRenderContext::GetVlkDevice();
	
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(device->GetHandle(), m_vkSwapchain, &imageCount, nullptr);

	std::vector<VkImage> swapchainImages(imageCount);
	vkGetSwapchainImagesKHR(device->GetHandle(), m_vkSwapchain, &imageCount, swapchainImages.data());

	VkImageViewCreateInfo imageViewInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = m_vkSurfaceFormat.format,
		.components = {VK_COMPONENT_SWIZZLE_IDENTITY},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	m_frameViews.resize(swapchainImages.size());

	for (uint32_t i = 0; i < m_frameViews.size(); i++) {
		imageViewInfo.image = swapchainImages[i];
		VkImageView frameView = VK_NULL_HANDLE;
		vkCreateImageView(device->GetHandle(), &imageViewInfo, nullptr, &frameView);
		m_frameViews[i] = frameView;
	}

	VkFramebufferCreateInfo framebufferInfo = {};
	VkSurfaceCapabilitiesKHR caps = device->GetActiveAdapter().GetSurfaceInfo(surface);

	framebufferInfo.width = caps.currentExtent.width;
	framebufferInfo.height = caps.currentExtent.height;
	framebufferInfo.layers = 1;
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.renderPass = m_vkRenderPass;

	m_framebuffers.resize(m_frameViews.size());

	for (uint32_t i = 0; i < m_frameViews.size(); i++) {

		VkImageView attachments[] = { m_frameViews[i] };
		framebufferInfo.pAttachments = attachments;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		vkCreateFramebuffer(device->GetHandle(), &framebufferInfo, nullptr, &framebuffer);

		m_framebuffers[i] = framebuffer;
	}

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &m_vkImageAvailableSemaphore);

}

PixelMachine::GPU::VlkSwapchain::~VlkSwapchain() {

	VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();

	vkDestroySemaphore(device, m_vkImageAvailableSemaphore, nullptr);

	for (auto fb : m_framebuffers) {
		vkDestroyFramebuffer(device, fb, nullptr);
	}

	for (auto view : m_frameViews) {
		vkDestroyImageView(device, view, nullptr);
	}

	if (m_vkSwapchain) {
		vkDestroySwapchainKHR(device, m_vkSwapchain, nullptr);
	}

	if (m_vkRenderPass) {
		vkDestroyRenderPass(device, m_vkRenderPass, nullptr);
	}

}

uint32_t PixelMachine::GPU::VlkSwapchain::GetImage(VkFramebuffer *outFramebuffer) const {

	VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();

	uint32_t index = 0u;
	vkAcquireNextImageKHR(device, m_vkSwapchain, UINT64_MAX, m_vkImageAvailableSemaphore, NULL, &index);

	if (outFramebuffer) {
		*outFramebuffer = m_framebuffers[index];
	}

	return index;
}
