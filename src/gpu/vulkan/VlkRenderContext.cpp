#include <vulkan/VlkRenderContext.h>
#include <vulkan/VlkDevice.h>
#include <vulkan/VlkSwapchain.h>
#include <vulkan/VlkShaderProgram.h>

#include <stdexcept>

namespace PixelMachine {
	namespace GPU {
		
		extern VlkRenderContext *s_vlkRenderContextP;

		VlkRenderContext::VlkRenderContext(HWND windowHandle) {

			if (!sm_vlkDeviceP) {
				sm_vlkDeviceP = new VlkDevice();
			}

			VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
			surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceInfo.pNext = nullptr;
			surfaceInfo.hinstance = GetModuleHandle(NULL);
			surfaceInfo.hwnd = windowHandle;

			vkCreateWin32SurfaceKHR(sm_vlkDeviceP->GetVkInstance(), &surfaceInfo, nullptr, &m_vkWinSurface);

			if (!m_vkWinSurface) {
				throw new std::runtime_error("VlkRenderContext init fail - cannot create VkSurface.");
			}

			m_vkWinSurfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
			m_vkWinSurfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

			bool adapterNotFound = true;
			for (uint32_t i = 0; i < sm_vlkDeviceP->GetAdapterCount(); i++) {
				VlkAdapter adapter = sm_vlkDeviceP->GetAdapter(i);
				if (adapter.SurfaceFormatAvailable(m_vkWinSurface, m_vkWinSurfaceFormat) &&
					adapter.PresentModeAvailable(m_vkWinSurface, VK_PRESENT_MODE_FIFO_KHR)) {
					sm_vlkDeviceP->SetAdapter(i);
					adapterNotFound = false;
					break;
				}
			}

			if (adapterNotFound) {
				throw new std::runtime_error("VlkRenderContext init fail - cannot find suitable physical device.");
			}

			m_vlkSwapchainP = new VlkSwapchain(m_vkWinSurface, m_vkWinSurfaceFormat, VK_PRESENT_MODE_FIFO_KHR);

		}

		VlkRenderContext::~VlkRenderContext() {

			m_vlkPasses.clear();

			if (m_vlkSwapchainP) {
				delete m_vlkSwapchainP;
			}

			if (m_vkWinSurface) {
				vkDestroySurfaceKHR(sm_vlkDeviceP->GetVkInstance(), m_vkWinSurface, nullptr);
			}

			if (sm_vlkDeviceP) {
				delete sm_vlkDeviceP;
				sm_vlkDeviceP = nullptr;
			}
		}

		void VlkRenderContext::EndPass() {

			if (!m_vlkPasses.size())
				return;

			VlkPass &newPass = *m_vlkPasses.rbegin();

			VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
			vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputStateInfo.vertexBindingDescriptionCount = 0;
			vertexInputStateInfo.pVertexBindingDescriptions = nullptr;
			vertexInputStateInfo.vertexAttributeDescriptionCount = 0;
			vertexInputStateInfo.pVertexAttributeDescriptions = nullptr;
	
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
			inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyInfo.topology = VkPrimitiveTopology(newPass.m_primitiveTopology);
			inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport = {};
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			if (newPass.m_renderToScreen) {

				VlkAdapter adapter = VlkRenderContext::GetVlkDevice()->GetActiveAdapter();
				VkSurfaceCapabilitiesKHR surfaceCaps = adapter.GetSurfaceInfo(m_vkWinSurface);

				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = static_cast<float>(surfaceCaps.currentExtent.width);
				viewport.height = static_cast<float>(surfaceCaps.currentExtent.height);
			}
			else {
				viewport.x = newPass.m_viewportRect[0];
				viewport.y = newPass.m_viewportRect[1];
				viewport.width = newPass.m_viewportRect[2];
				viewport.height = newPass.m_viewportRect[3];
			}

			
			VkRect2D scissorRect = {}; // Render full viewport 
			scissorRect.offset = { 0,0 };
			scissorRect.extent.width = viewport.width;
			scissorRect.extent.height = viewport.height;
			

			VkPipelineViewportStateCreateInfo viewportInfo = {};
			viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportInfo.viewportCount = 1;
			viewportInfo.pViewports = &viewport;
			viewportInfo.scissorCount = 1;
			viewportInfo.pScissors = &scissorRect;

			VkPipelineRasterizationStateCreateInfo rasterInfo = {};
			rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterInfo.depthClampEnable = VK_FALSE;
			rasterInfo.rasterizerDiscardEnable = VK_FALSE;
			rasterInfo.polygonMode = VK_POLYGON_MODE_FILL; // This should be set according to a primitive type
			rasterInfo.lineWidth = newPass.m_lineWidth;
			rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterInfo.depthBiasEnable = VK_FALSE;
			rasterInfo.depthBiasConstantFactor = 0.0f;
			rasterInfo.depthBiasClamp = 0.0f;
			rasterInfo.depthBiasSlopeFactor = 0.0f;

			VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
			multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisamplingInfo.sampleShadingEnable = VK_FALSE;
			multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisamplingInfo.minSampleShading = 1.0f;
			multisamplingInfo.pSampleMask = nullptr;
			multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
			multisamplingInfo.alphaToOneEnable = VK_FALSE;
			
			VkPipelineColorBlendAttachmentState colorBlendAttchState = {};
			colorBlendAttchState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
				VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttchState.blendEnable = VK_FALSE;
			colorBlendAttchState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttchState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttchState.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttchState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttchState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttchState.alphaBlendOp = VK_BLEND_OP_ADD;

			VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
			colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendInfo.logicOpEnable = VK_FALSE;
			colorBlendInfo.attachmentCount = 1;
			colorBlendInfo.pAttachments = &colorBlendAttchState;

			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = newPass.m_shaderStagesInfo.size();
			pipelineInfo.pStages = newPass.m_shaderStagesInfo.data();
			pipelineInfo.pVertexInputState = &vertexInputStateInfo;
			pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
			pipelineInfo.pViewportState = &viewportInfo;
			pipelineInfo.pRasterizationState = &rasterInfo;
			pipelineInfo.pMultisampleState = &multisamplingInfo;
			pipelineInfo.pDepthStencilState = nullptr;
			pipelineInfo.pColorBlendState = &colorBlendInfo;
			pipelineInfo.renderPass = m_vlkSwapchainP->GetVkRenderPass();
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex = -1;
			
			VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
			dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateInfo.dynamicStateCount = 2;
			VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_LINE_WIDTH };
			dynamicStateInfo.pDynamicStates = dynamicStates;
			pipelineInfo.pDynamicState = &dynamicStateInfo;
			
			VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();

			vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &newPass.m_vkPipelineLayout);

			pipelineInfo.layout = newPass.m_vkPipelineLayout;

			vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineInfo, nullptr, &newPass.m_vkPipeline);

		}

		VlkRenderContext::VlkPass::~VlkPass() {

			VkDevice device = VlkRenderContext::GetVlkDevice()->GetHandle();

			if (m_vkPipeline) {
				vkDestroyPipeline(device, m_vkPipeline, nullptr);
			}
			if (m_vkPipelineLayout) {
				vkDestroyPipelineLayout(device, m_vkPipelineLayout, nullptr);
			}

		}

		void VlkRenderContext::BindShaderProgram(const VlkShaderProgram *shaderProgram) {

			if (!shaderProgram || !m_vlkPasses.size()) {
				return;
			}

			VkPipelineShaderStageCreateInfo shaderInfo = {};
			shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderInfo.pName = "main";
			shaderInfo.module = shaderProgram->GetHandle();

			switch (shaderProgram->GetType())
			{
			case ShaderType::VertexShader:
				shaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case ShaderType::FragmentShader:
				shaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			case ShaderType::ComputeShader:
				shaderInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				break;
			default:
				break;
			}

			VlkPass &newPass = *m_vlkPasses.rbegin();
			newPass.m_shaderStagesInfo.push_back(shaderInfo);

		}
	}
}