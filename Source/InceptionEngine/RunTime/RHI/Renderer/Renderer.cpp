
#include "Renderer.h"

/*
External libraries
*/
#define STB_IMAGE_IMPLEMENTATION
#include "External/stb/stb_image.h"
#include "ThirdPartyLibraries/glfw/include/GLFW/glfw3.h"

#include "IRenderObject.h"
#include "RunTime/RHI/WindowHandler/WindowHandler.h"

#include "PathHelper.h"

#include <fstream>

namespace inceptionengine
{
	void Renderer::CreateUniformBufferDescription(UniformBufferDescription& uBufferDesc,
												  UniformBuffer const& uBuffer,
												  Texture const& texture)
	{
		uBufferDesc.pool = CreateDescriptorPool();
		uBufferDesc.layout = CreateDescriptorSetLayout();
		uBufferDesc.descriptorSet = CreateDescriptorSet(uBufferDesc.layout, uBufferDesc.pool, uBuffer, texture);
	}

	void Renderer::CreateSkyboxDataDescription(UniformBufferDescription& desc, UniformBuffer const& uBuffer, CubeMap const& cubemap)
	{
		desc.pool = CreateDescriptorPool();
		desc.layout = CreateSkyboxDescriptorSetLayout();
		desc.descriptorSet = CreateSkyboxDescriptorSet(desc.layout, desc.pool, uBuffer, cubemap);
	}

	void Renderer::DestroyVertexBuffer(VertexBuffer& vertexBuffer)
	{
		vkDeviceWaitIdle(m_device);
		vkDestroyBuffer(m_device, vertexBuffer.vertexBuffer, nullptr);
		vkFreeMemory(m_device, vertexBuffer.vertexBufferMemory, nullptr);
	}

	void Renderer::DestroyIndexBuffer(IndexBuffer& indexBuffer)
	{
		vkDeviceWaitIdle(m_device);
		vkDestroyBuffer(m_device, indexBuffer.indexBuffer, nullptr);
		vkFreeMemory(m_device, indexBuffer.indexBufferMemory, nullptr);
	}

	void Renderer::DestroyTexture(Texture& texture)
	{
		vkDeviceWaitIdle(m_device);
		vkDestroyImageView(m_device, texture.view, nullptr);
		vkDestroyImage(m_device, texture.textureImage, nullptr);
		vkFreeMemory(m_device, texture.textureImageMemory, nullptr);
		vkDestroySampler(m_device, texture.sampler, nullptr);
	}

	void Renderer::DestroyCubeMap(CubeMap& cubeMap)
	{
		vkDeviceWaitIdle(m_device);
		vkDestroyImageView(m_device, cubeMap.view, nullptr);
		vkDestroyImage(m_device, cubeMap.cubeMapImage, nullptr);
		vkFreeMemory(m_device, cubeMap.cubeMapImageMemory, nullptr);
		vkDestroySampler(m_device, cubeMap.sampler, nullptr);
	}

	void Renderer::DestroyUniformBufferDescription(UniformBufferDescription& desc)
	{
		vkDeviceWaitIdle(m_device);
		vkDestroyDescriptorPool(m_device, desc.pool, nullptr);
		vkDestroyDescriptorSetLayout(m_device, desc.layout, nullptr);
	}

	void Renderer::DestroyPipeline(Pipeline& pipeline)
	{
		vkDeviceWaitIdle(m_device);
		vkDestroyPipeline(m_device, pipeline.pipeline, nullptr);
		vkDestroyPipelineLayout(m_device, pipeline.pipelineLayout, nullptr);
	}

	void Renderer::DestroyUniformBuffer(UniformBuffer& uBuffer)
	{
		vkDeviceWaitIdle(m_device);
		for (unsigned int i = 0; i < NumOfRenderBuffers; i++)
		{
			vkDestroyBuffer(m_device, uBuffer.boneTransform[i], nullptr);
			vkFreeMemory(m_device, uBuffer.boneTransformMemory[i], nullptr);

			vkDestroyBuffer(m_device, uBuffer.light[i], nullptr);
			vkFreeMemory(m_device, uBuffer.lightMemory[i], nullptr);
		}

	}

	std::vector<char> Renderer::LoadShaderBinary(const std::string& filePath)
	{
		std::ifstream file(PathHelper::GetAbsolutePath(filePath), std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Cannot open file " << PathHelper::GetAbsolutePath(filePath) << std::endl;
			throw std::runtime_error("");
		}
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> binary(fileSize);
		file.seekg(0);
		file.read(binary.data(), fileSize);
		file.close();
		return binary;

	}

	void Renderer::DrawFrame()
	{
		if (isChangingSize || m_minimized)
		{
			return;
		}

		if (vkAcquireNextImageKHR(m_device, mSwapchain, UINT64_MAX, VK_NULL_HANDLE, m_acquireImageFence, &m_nextRenderImgIdx) != VK_SUCCESS)
		{
			std::cerr << "Fail to get next available image from the swap chain" << std::endl;
			throw std::runtime_error("");
		}

		vkWaitForFences(m_device, 1, &m_acquireImageFence, VK_TRUE, UINT64_MAX);

		vkResetFences(m_device, 1, &m_acquireImageFence);

		vkWaitForFences(m_device, 1, &m_imageFinishRenderFence[m_nextRenderImgIdx], VK_TRUE, UINT64_MAX);

		vkResetFences(m_device, 1, &m_imageFinishRenderFence[m_nextRenderImgIdx]);

		RecordCommandBuffer(m_nextRenderImgIdx);

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[m_nextRenderImgIdx];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_graphicsFinishSemaphores[m_nextRenderImgIdx];
		vkQueueSubmit(m_queues.at(GraphicsQueue), 1, &submitInfo, m_imageFinishRenderFence[m_nextRenderImgIdx]);

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_graphicsFinishSemaphores[m_nextRenderImgIdx];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &mSwapchain;
		presentInfo.pImageIndices = &m_nextRenderImgIdx;
		vkQueuePresentKHR(m_queues.at(PresentQueue), &presentInfo);

	}



	void Renderer::Initialize(WindowHandler& windowHandler)
	{
		CreateInstance();

		CreateSurface(windowHandler);

		CreatePhysicalDevice();

		CreateLogicalDevice();

		CreateSwapchain();

		CreateSwapchainImageViews();

		CreateDepthResources();

		CreateCommandPool();

		CreateRenderPass();

		CreateFramebuffer();

		CreateSemaphoreAndFence();

		CreateCommandBuffers();

	}

	RenderObejctID Renderer::SubmitToDevice(IRenderObject* renderComponent)
	{
		size_t renderObjectID = m_actors.size();
		m_actorLock.lock();
		m_actors.push_back(renderComponent);
		m_actorLock.unlock();
		return renderObjectID;
	}



	void Renderer::CreateTexture(Texture& texture, std::string filePath)
	{
		int width, height, nChannel;
		stbi_uc* pixels = stbi_load(PathHelper::GetAbsolutePath(filePath).c_str(), &width, &height, &nChannel, STBI_rgb_alpha);
		if (pixels == nullptr)
		{
			std::cerr << "Fail to load texture!" << std::endl;
			throw std::runtime_error("");
		}


		VkDeviceSize imageSize = static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * 4;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;
		CreateBuffer(stagingBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, imageSize, stagingBufferMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		void* data;
		vkMapMemory(m_device, stagingBufferMem, 0, imageSize, 0, &data);
		memcpy(data, reinterpret_cast<const void*>(pixels), static_cast<size_t>(imageSize));
		vkUnmapMemory(m_device, stagingBufferMem);
		stbi_image_free(pixels);

		CreateImage(texture.textureImage,
					texture.textureImageMemory,
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height),
					1,
					1,
					1,
					0,
					VK_IMAGE_TYPE_2D,
					VK_FORMAT_R8G8B8A8_UNORM,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		TransitImageLayout(texture.textureImage, 1, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

		VkBufferImageCopy region = {};
		region.bufferImageHeight = 0;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;

		region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
		region.imageOffset = { 0,0,0 };
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageSubresource.mipLevel = 0;

		auto commandBuffer = BeginSingleTimeCommandBuffer();
		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, texture.textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		EndSingleTimeCommandBuffer(commandBuffer);

		TransitImageLayout(texture.textureImage, 1, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

		vkDestroyBuffer(m_device, stagingBuffer, nullptr);
		vkFreeMemory(m_device, stagingBufferMem, nullptr);

		texture.view = CreateImageView(texture.textureImage, 1, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_2D);

		texture.sampler = CreateTextureSampler();
	}



	void Renderer::CreateInstance()
	{
		VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

		uint32_t enableExtensionCount = 0;
		const char** enableExtensionNames = glfwGetRequiredInstanceExtensions(&enableExtensionCount);


		const int enableLayerCount = 1;
		const char* enableLayerNames[enableLayerCount] = { "VK_LAYER_KHRONOS_validation" };
		VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		const char* appName = "Inception";
		appInfo.pApplicationName = appName;
		appInfo.apiVersion = VK_API_VERSION_1_2;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = enableExtensionCount; 
		createInfo.ppEnabledExtensionNames = enableExtensionNames;
		createInfo.enabledLayerCount = enableLayerCount;
		createInfo.ppEnabledLayerNames = enableLayerNames;
		try
		{
			if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
			{
				throw - 1;
			}

		}
		catch(int e)
		{
			createInfo.enabledLayerCount = 0;
			if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
			{
				std::cerr << "Fail to create vulkan instance!" << std::endl;
				throw std::runtime_error("");
			}

		}

	}

	void Renderer::CreatePhysicalDevice()
	{
		uint32_t physicalDeviceCount = 0;
		std::vector<VkPhysicalDevice> physcialDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);
		physcialDevices.resize(physicalDeviceCount);

		vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physcialDevices.data());

		for (const auto& device : physcialDevices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				std::cout << "Select discrete card: " << deviceProperties.deviceName << std::endl;
				m_physicalDevice = device;
				vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemProp);
				return;
			}

		}

		std::cout << "Select integrate card" << std::endl;
		m_physicalDevice = physcialDevices[0];

		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemProp);
	}

	bool Renderer::CheckPhysicalDeviceExtensionSupport()
	{
		uint32_t extensionCount;
		std::vector<VkExtensionProperties> supportedExtensions;
		vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, nullptr);
		supportedExtensions.resize(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, supportedExtensions.data());
		std::set<std::string> extensionsToCheck(DeviceExtensions.begin(), DeviceExtensions.end());
		for (const auto& extension : supportedExtensions)
		{
			extensionsToCheck.erase(extension.extensionName);
		}
		return extensionsToCheck.empty();
	}

	void Renderer::GetQueueFamilyIndices()
	{
		QueueFamilyIndices indices;
		uint32_t count = 0;
		std::vector<VkQueueFamilyProperties> properties(count);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, nullptr);
		properties.resize(count);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, properties.data());


		uint32_t i = 0;
		for (const auto& deviceProperty : properties)
		{
			if (deviceProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamilyIndex = i;
				break;
			}
			i++;
		}

		i = 0;
		VkBool32 support = false;
		for (const auto& deviceProperty : properties)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, m_surface, &support);
			if (support)
			{
				indices.presentFamilyIndex = i;
				break;
			}
			i++;
		}
		m_queueFamilyIndices = indices;
		m_queueFamilyIndices = indices;
	}

	void Renderer::CreateLogicalDevice()
	{
		if (!CheckPhysicalDeviceExtensionSupport())
		{
			std::cerr << "Your video card does not support swapchain extension!" << std::endl;
			throw std::runtime_error("");
		}

		GetQueueFamilyIndices();

		if (!m_queueFamilyIndices.IsComplete())
		{
			std::cerr << "Your video card does not support both rendering and presenting!" << std::endl;
			throw std::runtime_error("");
		}

		VkDeviceQueueCreateInfo deviceQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamilyIndex.value();
		float queuePriorty = 1.0f;
		deviceQueueCreateInfo.pQueuePriorities = &queuePriorty;
		VkDevice device = 0;

		VkPhysicalDeviceFeatures enabledFeatures = {};
		enabledFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
		createInfo.pEnabledFeatures = &enabledFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = DeviceExtensions.data();


		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		{
			std::cerr << "Fail to create logical device!" << std::endl;
			throw std::runtime_error("");
		}

		VkQueue graphicsQueue = nullptr;
		vkGetDeviceQueue(m_device, m_queueFamilyIndices.graphicsFamilyIndex.value(), 0, &graphicsQueue);
		if (graphicsQueue == nullptr)
		{
			std::cerr << "Fail to get graphics queue!" << std::endl;
			throw std::runtime_error("");
		}
		m_queues.insert(std::pair(GraphicsQueue, graphicsQueue));

		VkQueue presentQueue = nullptr;
		vkGetDeviceQueue(m_device, m_queueFamilyIndices.presentFamilyIndex.value(), 0, &presentQueue);
		if (presentQueue == nullptr)
		{
			std::cerr << "Fail to get graphics queue!" << std::endl;
			throw std::runtime_error("");
		}
		m_queues.insert(std::pair(PresentQueue, presentQueue));
	}

	void Renderer::CreateSurface(WindowHandler& windowHandler)
	{
		m_windowHeight = windowHandler.GetWindowHeight();
		m_windowWidth = windowHandler.GetWindowWidth();

		if (glfwCreateWindowSurface(m_instance, windowHandler.GetWindow(), nullptr, &m_surface))
		{
			std::cerr << "Fail to create vulkan surface!" << std::endl;
			throw std::runtime_error("");
		}
	}

	void Renderer::CreateSwapchain()
	{
		m_imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		createInfo.surface = m_surface;
		createInfo.minImageCount = NumOfRenderBuffers;
		createInfo.imageFormat = m_imageFormat; //SHORTCUT!!
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		mSwapchainExtent = { m_windowWidth, m_windowHeight };
		m_mvp.view = LookAt(Vec3f(0.0f, 80.0f, 80.0f), Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
		m_mvp.proj = Perspective(45.0f, static_cast<float>(mSwapchainExtent.width) / static_cast<float>(mSwapchainExtent.height),
								 0.1f, 1000000.0f);
		createInfo.imageExtent = mSwapchainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (m_queueFamilyIndices.graphicsFamilyIndex.value() == m_queueFamilyIndices.presentFamilyIndex.value())
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		else
		{
			std::cout << "The graphics queue and present queue are different" << std::endl;
			std::cout << "The index of graphcis queue is " << m_queueFamilyIndices.graphicsFamilyIndex.value() << std::endl;
			std::cout << "The index of present queue is " << m_queueFamilyIndices.presentFamilyIndex.value() << std::endl;
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			const uint32_t indices[] = { m_queueFamilyIndices.graphicsFamilyIndex.value(), m_queueFamilyIndices.presentFamilyIndex.value() };
			createInfo.pQueueFamilyIndices = indices;

		}

		createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.clipped = VK_TRUE;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &mSwapchain) != VK_SUCCESS)
		{
			std::cerr << "fail to create swapchain! Error code is: " << std::endl;
			throw std::runtime_error("");
		}

		uint32_t swapchainImageCount = 0;
		vkGetSwapchainImagesKHR(m_device, mSwapchain, &swapchainImageCount, nullptr);
		mSwapchainImages.resize(swapchainImageCount);
		if (vkGetSwapchainImagesKHR(m_device, mSwapchain, &swapchainImageCount, mSwapchainImages.data()) != VK_SUCCESS)
		{
			std::cerr << "Fail to get swapchin images!" << std::endl;
			throw std::runtime_error("");
		}
	}

	void Renderer::CreateSwapchainImageViews()
	{
		mSwapchainImageViews.resize(mSwapchainImages.size());
		for (size_t i = 0; i < mSwapchainImageViews.size(); i++)
		{
			VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			createInfo.image = mSwapchainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_imageFormat;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.subresourceRange.levelCount = 1;

			if (vkCreateImageView(m_device, &createInfo, nullptr, &mSwapchainImageViews[i]) != VK_SUCCESS)
			{
				std::cerr << "Fail to create " << i << " image view!" << std::endl;
				throw std::runtime_error("");
			}
		}
	}

	void Renderer::CreateDepthResources()
	{
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

		CreateImage(mDepthImage,
					mDepthImageMem,
					m_windowWidth,
					m_windowHeight,
					1,
					1,
					1,
					0,
					VK_IMAGE_TYPE_2D,
					depthFormat,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		mDepthImageView = CreateImageView(mDepthImage, 1, VK_IMAGE_ASPECT_DEPTH_BIT, depthFormat, VK_IMAGE_VIEW_TYPE_2D);
	}

	void Renderer::CreateFramebuffer()
	{
		m_framebuffers.resize(mSwapchainImageViews.size());
		for (uint32_t i = 0; i < m_framebuffers.size(); i++)
		{
			std::array<VkImageView, 2> attachments = { mSwapchainImageViews[i], mDepthImageView };

			VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			createInfo.width = mSwapchainExtent.width;
			createInfo.height = mSwapchainExtent.height;
			createInfo.layers = 1;
			createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
			createInfo.pAttachments = attachments.data();;
			assert(m_renderPass);
			createInfo.renderPass = m_renderPass;
			if (vkCreateFramebuffer(m_device, &createInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
			{
				std::cerr << "Fail to create framebuffer " << i << std::endl;
				throw std::runtime_error("");
			}
		}
	}

	void Renderer::CreateCommandPool()
	{
		VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		createInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamilyIndex.value();
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		if (vkCreateCommandPool(m_device, &createInfo, nullptr, &m_commandPool) != VK_SUCCESS)
		{
			std::cerr << "Fail to create command pool!" << std::endl;
			throw std::runtime_error("");
		}
	}


	void Renderer::CreateCommandBuffers()
	{

		m_commandBuffers.resize(mSwapchainImages.size());
		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = m_commandPool;
		allocateInfo.commandBufferCount = (uint32_t)mSwapchainImages.size();
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		if (vkAllocateCommandBuffers(m_device, &allocateInfo, m_commandBuffers.data()) != VK_SUCCESS)
		{
			std::cerr << "Fail to allocate command buffers!" << std::endl;
			throw std::runtime_error("");
		}
	}



	void Renderer::RecordCommandBuffer(uint32_t i)
	{

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo);

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.5f, 0.5f, 0.5f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBeginInfo.renderPass = m_renderPass;
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = mSwapchainExtent;
		renderPassBeginInfo.framebuffer = m_framebuffers[i];
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();;

		vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkDeviceSize offset[] = { 0 };

		for (auto& renderComponent : m_actors)
		{
			auto& renderUnits = renderComponent->GetRenderUnits(i);
			for (auto& renderUnit : renderUnits)
			{
				if (renderUnit.visible)
				{
					vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderUnit.pipeline);

					vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, renderUnit.vertexBuffer, offset);

					vkCmdBindIndexBuffer(m_commandBuffers[i], renderUnit.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

					vkCmdPushConstants(m_commandBuffers[i], renderUnit.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MVP), (void*)&m_mvp);

					vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderUnit.pipelineLayout, 0, 1, renderUnit.descriptorSet, 0, nullptr);

					vkCmdDrawIndexed(m_commandBuffers[i], renderUnit.numOfIndices, 1, 0, 0, 0);
				}
			}

		}


		vkCmdEndRenderPass(m_commandBuffers[i]);

		vkEndCommandBuffer(m_commandBuffers[i]);

	}

	void Renderer::CreateSemaphoreAndFence()
	{

		VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		m_graphicsFinishSemaphores.resize((size_t)NumOfRenderBuffers);
		m_imageFinishRenderFence.resize((size_t)NumOfRenderBuffers);

		for (size_t i = 0; i < NumOfRenderBuffers; i++)
		{
			vkCreateSemaphore(m_device, &createInfo, nullptr, &m_graphicsFinishSemaphores[i]);
			vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_imageFinishRenderFence[i]);
		}

		fenceCreateInfo.flags = 0;
		vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_acquireImageFence);
	}

	VkShaderModule Renderer::LoadShaderModule(const std::string& filePath)
	{
		auto code = LoadShaderBinary(filePath);
		VkShaderModule shaderModule;
		VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		createInfo.codeSize = (uint32_t)code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule);
		return shaderModule;

	}



	void Renderer::CreatePipeline(Pipeline& pipeline, ShaderPath const& shaderpath, UniformBufferDescription& dataDesc, bool useDepthTest, VkPrimitiveTopology topology)
	{

		VkShaderModule vertexShader = LoadShaderModule(shaderpath.vertexShaderPath);
		VkShaderModule fragmentShader = LoadShaderModule(shaderpath.fragmentShaderPath);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShader;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragmentShader;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStageInfos[] = { vertShaderStageInfo, fragShaderStageInfo };


		//fix functions
		VkPipelineVertexInputStateCreateInfo vertInputStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		auto vertexAttriDesc = GetVertexAttriDesc();
		vertInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttriDesc.size();
		vertInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttriDesc.data();
		vertInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		VkVertexInputBindingDescription vertexBindingDesc = GetVertexBindingDesc();
		vertInputStateCreateInfo.pVertexBindingDescriptions = &vertexBindingDesc;


		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssemblyStateCreateInfo.topology = topology;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.width = (float)m_windowWidth;
		viewport.height = -(float)m_windowHeight;
		viewport.x = 0.0f;
		viewport.y = (float)m_windowHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0,0 };
		scissor.extent = mSwapchainExtent;

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.lineWidth = 1.0f;
		//two-sided or single-sided of the material
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachmentState.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional


		//pipeline layout
		VkPushConstantRange pushConstRange = {};
		pushConstRange.offset = 0;
		pushConstRange.size = sizeof(MVP);
		pushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;


		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstRange;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &dataDesc.layout;
		if (vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &(pipeline.pipelineLayout)) != VK_SUCCESS)
		{
			std::cerr << "Fail to create pipeline layout!" << std::endl;
			throw std::runtime_error("");
		}


		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		graphicsPipelineCreateInfo.stageCount = 2;
		graphicsPipelineCreateInfo.pStages = shaderStageInfos;
		graphicsPipelineCreateInfo.pVertexInputState = &vertInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.layout = pipeline.pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = m_renderPass;
		graphicsPipelineCreateInfo.subpass = 0; //index of the subpass that this graphics pipeline will use
		if (useDepthTest) graphicsPipelineCreateInfo.pDepthStencilState = &depthStencil;

		if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &(pipeline.pipeline)) != VK_SUCCESS)
		{
			std::cerr << "Fail to create graphics pipeline!" << std::endl;
			throw std::runtime_error("");
		}

		vkDestroyShaderModule(m_device, vertexShader, nullptr);
		vkDestroyShaderModule(m_device, fragmentShader, nullptr);
	}

	void Renderer::CleaupSwapChain()
	{
		//framebuffer
		for (size_t i = 0; i < NumOfRenderBuffers; i++)
		{
			vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
			vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffers[i]);
			vkDestroyImageView(m_device, mSwapchainImageViews[i], nullptr);
			vkDestroySemaphore(m_device, m_graphicsFinishSemaphores[i], nullptr);
			vkDestroyFence(m_device, m_imageFinishRenderFence[i], nullptr);
		}

		vkDestroyFence(m_device, m_acquireImageFence, nullptr);



		//renderpass
		vkDestroyRenderPass(m_device, m_renderPass, nullptr);

		//swapchian
		vkDestroySwapchainKHR(m_device, mSwapchain, nullptr);

	}



	void Renderer::CreateBuffer(VkBuffer& buffer,
								VkBufferUsageFlags usageFlags,
								VkDeviceSize size,
								VkDeviceMemory& memory,
								VkMemoryPropertyFlags memProperties)
	{
		VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		createInfo.usage = usageFlags;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.size = size;

		if (vkCreateBuffer(m_device, &createInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			std::cerr << "Fail to create vertex buffer!" << std::endl;
			throw std::runtime_error("");
		}


		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements(m_device, buffer, &memReq);
		VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex = FindSuitableMemType(memReq.memoryTypeBits, memProperties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
		{
			std::cerr << "Fail to allocate memory for vertex buffer!" << std::endl;
			throw std::runtime_error("");
		}

		vkBindBufferMemory(m_device, buffer, memory, 0);
	}

	void Renderer::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
	{
		VkCommandBuffer copyCmdBuffer;
		VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		if (vkAllocateCommandBuffers(m_device, &allocInfo, &copyCmdBuffer) != VK_SUCCESS)
		{
			std::cerr << "Fail to allocate copy command buffer!" << std::endl;
			throw std::runtime_error("");
		}
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };

		if (vkBeginCommandBuffer(copyCmdBuffer, &beginInfo) != VK_SUCCESS)
		{
			std::cerr << "Fail to begin copy command buffer!" << std::endl;
			throw std::runtime_error("");
		}

		VkBufferCopy copyInfo;
		copyInfo.size = size;
		copyInfo.srcOffset = 0;
		copyInfo.dstOffset = 0;

		vkCmdCopyBuffer(copyCmdBuffer, src, dst, 1, &copyInfo);

		if (vkEndCommandBuffer(copyCmdBuffer) != VK_SUCCESS)
		{
			std::cerr << "Fail to end copy command buffer!" << std::endl;
			throw std::runtime_error("");
		}

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &copyCmdBuffer;
		vkQueueSubmit(m_queues.at(GraphicsQueue), 1, &submitInfo, VK_NULL_HANDLE);
		vkDeviceWaitIdle(m_device);
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &copyCmdBuffer);

	}



	void Renderer::CreateVertexBuffer(VertexBuffer& vertexBuffer, std::vector<Vertex> const& vertices)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;
		VkDeviceSize size = sizeof(Vertex) * vertices.size();

		CreateBuffer(stagingBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, stagingBufferMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data = nullptr;
		vkMapMemory(m_device, stagingBufferMem, 0, size, 0, &data);

		memcpy(data, vertices.data(), (size_t)size);

		vkUnmapMemory(m_device, stagingBufferMem);

		CreateBuffer(vertexBuffer.vertexBuffer, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size, vertexBuffer.vertexBufferMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CopyBuffer(stagingBuffer, vertexBuffer.vertexBuffer, size);

		vkDestroyBuffer(m_device, stagingBuffer, nullptr);

		vkFreeMemory(m_device, stagingBufferMem, nullptr);

	}



	void Renderer::CreateIndexBuffer(IndexBuffer& indexBuffer, std::vector<uint32_t> const& indices)
	{

		VkDeviceSize size = indices.size() * sizeof(uint32_t);
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;
		CreateBuffer(stagingBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, stagingBufferMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(m_device, stagingBufferMem, 0, size, 0, &data);
		memcpy(data, indices.data(), size);
		vkUnmapMemory(m_device, stagingBufferMem);

		CreateBuffer(indexBuffer.indexBuffer, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, size, indexBuffer.indexBufferMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CopyBuffer(stagingBuffer, indexBuffer.indexBuffer, size);

		vkDestroyBuffer(m_device, stagingBuffer, nullptr);
		vkFreeMemory(m_device, stagingBufferMem, nullptr);

	}




	void Renderer::CreateImage(VkImage& image, VkDeviceMemory& imageMem, uint32_t width, uint32_t height, uint32_t depth, uint32_t arrayLayers, uint32_t mipLevels, VkImageCreateFlags flags, VkImageType type, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryProperties)
	{
		VkImageCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		createInfo.flags = flags;
		createInfo.arrayLayers = arrayLayers;
		createInfo.extent.width = width;
		createInfo.extent.height = height;
		createInfo.extent.depth = depth;
		createInfo.format = format;
		createInfo.imageType = type;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.mipLevels = mipLevels;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.tiling = tiling;
		createInfo.usage = usage;

		if (vkCreateImage(m_device, &createInfo, nullptr, &image) != VK_SUCCESS)
		{
			std::cerr << "Fail to create image!" << std::endl;
			throw std::runtime_error("");
		}

		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements(m_device, image, &memReq);
		VkDeviceSize size = memReq.size;

		VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocInfo.allocationSize = size;
		allocInfo.memoryTypeIndex = FindSuitableMemType(memReq.memoryTypeBits, memoryProperties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMem) != VK_SUCCESS)
		{
			std::cerr << "Fail to allocate image memory!" << std::endl;
			throw std::runtime_error("");
		}
		vkBindImageMemory(m_device, image, imageMem, 0);

	}

	VkCommandBuffer Renderer::BeginSingleTimeCommandBuffer()
	{
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = m_commandPool;
		allocateInfo.commandBufferCount = 1;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vkAllocateCommandBuffers(m_device, &allocateInfo, &commandBuffer);
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;

	}

	void Renderer::EndSingleTimeCommandBuffer(VkCommandBuffer& buffer)
	{
		vkEndCommandBuffer(buffer);

		VkFence fence;
		VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence);

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer;
		vkQueueSubmit(m_queues.at(GraphicsQueue), 1, &submitInfo, fence);

		vkWaitForFences(m_device, 1, &fence, VK_TRUE, UINT64_MAX);
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &buffer);
		vkDestroyFence(m_device, fence, nullptr);

	}

	void Renderer::TransitImageLayout(VkImage& image,
									  uint32_t numLayers,
									  VkImageLayout oldLayout,
									  VkImageLayout newLayout,
									  VkAccessFlags srcAccessMask,
									  VkAccessFlags dstAccessMask,
									  VkImageAspectFlags aspectFlag,
									  VkPipelineStageFlags srcStageMask,
									  VkPipelineStageFlags dstStageMask)
	{
		VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.dstAccessMask = dstAccessMask;
		barrier.image = image;
		barrier.newLayout = newLayout;
		barrier.oldLayout = oldLayout;
		barrier.srcAccessMask = srcAccessMask;
		barrier.subresourceRange.aspectMask = aspectFlag;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.layerCount = numLayers;
		barrier.subresourceRange.levelCount = 1;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		auto commandBuffer = BeginSingleTimeCommandBuffer();
		vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		EndSingleTimeCommandBuffer(commandBuffer);
	}


	VkDescriptorSetLayout Renderer::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayout layout;

		std::array<VkDescriptorSetLayoutBinding, 3> descSetLayoutBinding = {};
		descSetLayoutBinding[0].binding = 0;
		descSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descSetLayoutBinding[0].descriptorCount = 1;
		descSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		descSetLayoutBinding[0].pImmutableSamplers = nullptr;

		descSetLayoutBinding[1].binding = 1;
		descSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descSetLayoutBinding[1].descriptorCount = 1;
		descSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descSetLayoutBinding[1].pImmutableSamplers = nullptr;

		descSetLayoutBinding[2].binding = 2;
		descSetLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descSetLayoutBinding[2].descriptorCount = 1;
		descSetLayoutBinding[2].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		descSetLayoutBinding[2].pImmutableSamplers = nullptr;


		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutCreateInfo.bindingCount = static_cast<uint32_t>(descSetLayoutBinding.size());
		layoutCreateInfo.pBindings = descSetLayoutBinding.data();

		if (vkCreateDescriptorSetLayout(m_device, &layoutCreateInfo, nullptr, &layout) != VK_SUCCESS)
		{
			std::cerr << "Fail to create descriptor set layout." << std::endl;
			throw std::runtime_error("");
		}

		return layout;
	}

	VkDescriptorSetLayout Renderer::CreateSkyboxDescriptorSetLayout()
	{
		VkDescriptorSetLayout layout;

		std::array<VkDescriptorSetLayoutBinding, 2> descSetLayoutBinding = {};
		descSetLayoutBinding[0].binding = 0;
		descSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descSetLayoutBinding[0].descriptorCount = 1;
		descSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descSetLayoutBinding[0].pImmutableSamplers = nullptr;


		descSetLayoutBinding[1].binding = 1;
		descSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descSetLayoutBinding[1].descriptorCount = 1;
		descSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		descSetLayoutBinding[1].pImmutableSamplers = nullptr;


		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutCreateInfo.bindingCount = static_cast<uint32_t>(descSetLayoutBinding.size());
		layoutCreateInfo.pBindings = descSetLayoutBinding.data();

		if (vkCreateDescriptorSetLayout(m_device, &layoutCreateInfo, nullptr, &layout) != VK_SUCCESS)
		{
			std::cerr << "Fail to create descriptor set layout." << std::endl;
			throw std::runtime_error("");
		}

		return layout;

	}


	VkDescriptorPool Renderer::CreateDescriptorPool()
	{
		VkDescriptorPool pool;
		std::array<VkDescriptorPoolSize, 3> descPoolSzie = {};
		descPoolSzie[0].descriptorCount = NumOfRenderBuffers;
		descPoolSzie[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		descPoolSzie[1].descriptorCount = NumOfRenderBuffers;
		descPoolSzie[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		descPoolSzie[2].descriptorCount = NumOfRenderBuffers;
		descPoolSzie[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;


		VkDescriptorPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		createInfo.poolSizeCount = static_cast<uint32_t>(descPoolSzie.size());
		createInfo.pPoolSizes = descPoolSzie.data();
		createInfo.maxSets = NumOfRenderBuffers;

		if (vkCreateDescriptorPool(m_device, &createInfo, nullptr, &pool) != VK_SUCCESS)
		{
			std::cerr << "Fail to create descriptor pool." << std::endl;
			throw std::runtime_error("");
		}

		return pool;
	}



	/*
	Create and initialize uniform buffer with default pose and light
	*/

	void Renderer::CreateUniformBuffer(UniformBuffer& uBuffer)
	{
		constexpr VkDeviceSize boneTransformSize = sizeof(Matrix4x4f) * MaxBonePerSkeleton + sizeof(Matrix4x4f) * 2;
		constexpr VkDeviceSize lightPropertySize = sizeof(Light);

		for (unsigned int i = 0; i < NumOfRenderBuffers; i++)
		{
			CreateBuffer(uBuffer.boneTransform[i], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, boneTransformSize, uBuffer.boneTransformMemory[i], VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			CreateBuffer(uBuffer.light[i], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, lightPropertySize, uBuffer.lightMemory[i], VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}

		InitializeUniformBuffer(uBuffer);
	}



	void Renderer::UpdateUniformBuffer(UniformBuffer& uBuffer, const std::vector<Matrix4x4f>& transformations, Light* light)
	{
		//change the buffer of the image that is not being rendered now
		assert(transformations.size() >= AnimPoseOffsetInUBuffer);

		auto transformCopy = transformations;

		unsigned int writeToImageIdx = m_nextRenderImgIdx ^ 1;


		void* data = nullptr;

		vkMapMemory(m_device, uBuffer.boneTransformMemory[writeToImageIdx], 0, sizeof(Matrix4x4f) * transformations.size(), 0, &data);

		memcpy(data, transformCopy.data(), sizeof(Matrix4x4f) * transformations.size());

		vkUnmapMemory(m_device, uBuffer.boneTransformMemory[writeToImageIdx]);

		if (light != nullptr)
		{
			data = nullptr;

			vkMapMemory(m_device, uBuffer.lightMemory[writeToImageIdx], 0, sizeof(Light), 0, &data);
			memcpy(data, light, sizeof(Light));
			vkUnmapMemory(m_device, uBuffer.lightMemory[writeToImageIdx]);
		}


	}

	void Renderer::CreateCubeMap(CubeMap& cubeMap, std::array<std::string, 6> const& texturePaths)
	{
		int cubeMapWidth, cubeMapHeight, cubeMapNChannel;
		stbi_uc* front = stbi_load(PathHelper::GetAbsolutePath(texturePaths[0]).c_str(), &cubeMapWidth, &cubeMapHeight, &cubeMapNChannel, STBI_rgb_alpha);

		if (front == nullptr) throw std::runtime_error("");


		VkDeviceSize imageSize = static_cast<uint64_t>(cubeMapWidth) * static_cast<uint64_t>(cubeMapHeight) * 4;

		VkDeviceSize bufferSize = imageSize * 6;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;
		CreateBuffer(stagingBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, bufferSize, stagingBufferMem, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stbi_image_free(front);


		void* data;
		vkMapMemory(m_device, stagingBufferMem, 0, bufferSize, 0, &data);

		for (int i = 0; i < 6; i++)
		{
			int width, height, nChannel;
			stbi_uc* textureImage = stbi_load(PathHelper::GetAbsolutePath(texturePaths[i]).c_str(), &width, &height, &nChannel, STBI_rgb_alpha);
			if (textureImage == nullptr || width != cubeMapWidth || height != cubeMapHeight) throw std::runtime_error("");

			memcpy(static_cast<char*>(data) + i * imageSize, reinterpret_cast<const void*>(textureImage), static_cast<size_t>(imageSize));

			stbi_image_free(textureImage);
		}

		vkUnmapMemory(m_device, stagingBufferMem);



		CreateImage(cubeMap.cubeMapImage,
					cubeMap.cubeMapImageMemory,
					static_cast<uint32_t>(cubeMapWidth),
					static_cast<uint32_t>(cubeMapHeight),
					1,
					6,
					1,
					VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
					VK_IMAGE_TYPE_2D,
					VK_FORMAT_R8G8B8A8_UNORM,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		TransitImageLayout(cubeMap.cubeMapImage, 6, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

		VkBufferImageCopy region = {};
		region.bufferImageHeight = 0;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;

		region.imageExtent = { static_cast<uint32_t>(cubeMapWidth), static_cast<uint32_t>(cubeMapHeight), 1 };
		region.imageOffset = { 0,0,0 };
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 6;
		region.imageSubresource.mipLevel = 0;

		auto commandBuffer = BeginSingleTimeCommandBuffer();
		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, cubeMap.cubeMapImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		EndSingleTimeCommandBuffer(commandBuffer);

		TransitImageLayout(cubeMap.cubeMapImage, 6, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

		vkDestroyBuffer(m_device, stagingBuffer, nullptr);
		vkFreeMemory(m_device, stagingBufferMem, nullptr);

		cubeMap.view = CreateImageView(cubeMap.cubeMapImage, 6, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_CUBE);

		cubeMap.sampler = CreateTextureSampler();

	}

	//this function is different from the previous one by that it modified all of ubo in tBuffer
	void Renderer::InitializeUniformBuffer(UniformBuffer& uBuffer,
										   const std::vector<Matrix4x4f>& transformations,
										   Light* light)
	{
		//change the buffer of the image that is not being rendered now

		//auto transformCopy = transformations;

		for (unsigned int i = 0; i < NumOfRenderBuffers; i++)
		{
			void* data = nullptr;

			vkMapMemory(m_device, uBuffer.boneTransformMemory[i], 0, sizeof(Matrix4x4f) * transformations.size(), 0, &data);

			memcpy(data, transformations.data(), sizeof(Matrix4x4f) * transformations.size());

			vkUnmapMemory(m_device, uBuffer.boneTransformMemory[i]);

			if (light != nullptr)
			{
				data = nullptr;

				vkMapMemory(m_device, uBuffer.lightMemory[i], 0, sizeof(Light), 0, &data);

				memcpy(data, light, sizeof(Light));

				vkUnmapMemory(m_device, uBuffer.lightMemory[i]);
			}
			else
			{
				data = nullptr;

				Light light;

				light.m_locationAndIntensity = { 100.0f, 80.0f, 500.0f, 1.4f };

				vkMapMemory(m_device, uBuffer.lightMemory[i], 0, sizeof(Light), 0, &data);

				memcpy(data, &light, sizeof(Light));

				vkUnmapMemory(m_device, uBuffer.lightMemory[i]);

			}
		}
	}

	void Renderer::InitializeLight(UniformBuffer& uBuffer, Light* light)
	{
		void* data;

		for (unsigned int i = 0; i < NumOfRenderBuffers; i++)
		{
			data = nullptr;

			vkMapMemory(m_device, uBuffer.lightMemory[i], 0, sizeof(Light), 0, &data);

			memcpy(data, light, sizeof(Light));

			vkUnmapMemory(m_device, uBuffer.lightMemory[i]);

		}
	}


	void Renderer::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachmentDesc = {};
		colorAttachmentDesc.format = m_imageFormat;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachmentDesc = {};
		depthAttachmentDesc.format = VK_FORMAT_D32_SFLOAT;
		depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &colorAttachmentRef;
		subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


		std::array<VkAttachmentDescription, 2> attachments = { colorAttachmentDesc, depthAttachmentDesc };

		VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
		createInfo.pAttachments = attachments.data();
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpassDesc;
		createInfo.dependencyCount = 1;
		createInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_device, &createInfo, nullptr, &m_renderPass) != VK_SUCCESS)
		{
			std::cerr << "Fail to create render pass." << std::endl;
			throw std::runtime_error("");
		}

	}



	VkImageView Renderer::CreateImageView(VkImage& image, uint32_t numLayers, VkImageAspectFlags aspectMask, VkFormat viewFormat, VkImageViewType viewType)
	{
		VkImageView view;
		VkImageSubresourceRange range = {};
		range.aspectMask = aspectMask;
		range.baseArrayLayer = 0;
		range.baseMipLevel = 0;
		range.layerCount = numLayers;
		range.levelCount = 1;
		VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		createInfo.format = viewFormat;
		createInfo.image = image;
		createInfo.subresourceRange = range;
		createInfo.viewType = viewType;

		if (vkCreateImageView(m_device, &createInfo, nullptr, &view) != VK_SUCCESS)
		{
			std::cerr << "Fail to create image view." << std::endl;
			throw std::runtime_error("");
		}

		return view;

	}

	VkSampler Renderer::CreateTextureSampler()
	{
		VkSampler sampler;
		VkSamplerCreateInfo createInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.anisotropyEnable = VK_TRUE;
		createInfo.maxAnisotropy = 16;
		createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		createInfo.compareEnable = VK_FALSE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.magFilter = VK_FILTER_LINEAR;
		createInfo.minFilter = VK_FILTER_LINEAR;
		createInfo.maxLod = 0.0f;
		createInfo.minLod = 0.0f;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.mipLodBias = 0.0f;
		createInfo.unnormalizedCoordinates = VK_FALSE;

		if (vkCreateSampler(m_device, &createInfo, nullptr, &sampler) != VK_SUCCESS)
		{
			std::cerr << "Fail to create texture sampler." << std::endl;
			throw std::runtime_error("");
		}
		return sampler;
	}

	void Renderer::Destroy()
	{

		vkDeviceWaitIdle(m_device);

		for (const auto& framebuffer : m_framebuffers)
		{
			vkDestroyFramebuffer(m_device, framebuffer, nullptr);
		}

		vkDestroyRenderPass(m_device, m_renderPass, nullptr);



		for (const auto& imageView : mSwapchainImageViews)
		{
			vkDestroyImageView(m_device, imageView, nullptr);
		}

		vkDestroyImage(m_device, mDepthImage, nullptr);

		vkDestroyImageView(m_device, mDepthImageView, nullptr);

		vkFreeMemory(m_device, mDepthImageMem, nullptr);


		for (uint32_t i = 0; i < NumOfRenderBuffers; i++)
		{
			vkDestroySemaphore(m_device, m_graphicsFinishSemaphores[i], nullptr);
			vkDestroyFence(m_device, m_imageFinishRenderFence[i], nullptr);
		}

		vkDestroyFence(m_device, m_acquireImageFence, nullptr);

		vkDestroyCommandPool(m_device, m_commandPool, nullptr);

		vkDestroySwapchainKHR(m_device, mSwapchain, nullptr);

		vkDestroyDevice(m_device, nullptr);

		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

		vkDestroyInstance(m_instance, nullptr);
	}



	std::array<VkDescriptorSet, NumOfRenderBuffers> Renderer::CreateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool, const UniformBuffer& uBuffer, const Texture& texture)
	{
		std::array<VkDescriptorSet, NumOfRenderBuffers> set;

		std::vector<VkDescriptorSetLayout> layouts(NumOfRenderBuffers, layout);

		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = NumOfRenderBuffers;
		allocInfo.pSetLayouts = layouts.data();


		VkResult t;
		if ((t = vkAllocateDescriptorSets(m_device, &allocInfo, set.data())) != VK_SUCCESS)
		{
			std::cerr << "Fail to create descriptor sets." << std::endl;
			throw std::runtime_error("");
		}

		for (uint32_t i = 0; i < NumOfRenderBuffers; i++)
		{
			VkDescriptorBufferInfo boneTransformBufferInfo = {};
			boneTransformBufferInfo.offset = 0;
			boneTransformBufferInfo.buffer = uBuffer.boneTransform[i];
			boneTransformBufferInfo.range = VK_WHOLE_SIZE;

			VkDescriptorBufferInfo lightInfo = {};
			lightInfo.offset = 0;
			lightInfo.buffer = uBuffer.light[i];
			lightInfo.range = VK_WHOLE_SIZE;

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texture.view;
			imageInfo.sampler = texture.sampler;

			std::array<VkWriteDescriptorSet, 3> writeDescSet = {};
			writeDescSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet[0].descriptorCount = 1;
			writeDescSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescSet[0].dstArrayElement = 0;
			writeDescSet[0].dstBinding = 0;
			writeDescSet[0].dstSet = set[i];
			writeDescSet[0].pBufferInfo = &boneTransformBufferInfo;

			writeDescSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet[1].descriptorCount = 1;
			writeDescSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescSet[1].dstArrayElement = 0;
			writeDescSet[1].dstBinding = 1;
			writeDescSet[1].dstSet = set[i];
			writeDescSet[1].pImageInfo = &imageInfo;

			writeDescSet[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet[2].descriptorCount = 1;
			writeDescSet[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescSet[2].dstArrayElement = 0;
			writeDescSet[2].dstBinding = 2;
			writeDescSet[2].dstSet = set[i];
			writeDescSet[2].pBufferInfo = &lightInfo;

			vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writeDescSet.size()), writeDescSet.data(), 0, nullptr);
		}

		return set;
	}

	std::array<VkDescriptorSet, NumOfRenderBuffers> Renderer::CreateSkyboxDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool, const UniformBuffer& uBuffer, const CubeMap& cubemap)
	{
		std::array<VkDescriptorSet, NumOfRenderBuffers> set;

		std::vector<VkDescriptorSetLayout> layouts(NumOfRenderBuffers, layout);

		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = NumOfRenderBuffers;
		allocInfo.pSetLayouts = layouts.data();


		VkResult t;
		if ((t = vkAllocateDescriptorSets(m_device, &allocInfo, set.data())) != VK_SUCCESS)
		{
			std::cerr << "Fail to create descriptor sets." << std::endl;
			throw std::runtime_error("");
		}

		for (uint32_t i = 0; i < NumOfRenderBuffers; i++)
		{

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = cubemap.view;
			imageInfo.sampler = cubemap.sampler;

			VkDescriptorBufferInfo lightInfo = {};
			lightInfo.offset = 0;
			lightInfo.buffer = uBuffer.light[i];
			lightInfo.range = VK_WHOLE_SIZE;


			std::array<VkWriteDescriptorSet, 2> writeDescSet = {};

			writeDescSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet[0].descriptorCount = 1;
			writeDescSet[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescSet[0].dstArrayElement = 0;
			writeDescSet[0].dstBinding = 0;
			writeDescSet[0].dstSet = set[i];
			writeDescSet[0].pImageInfo = &imageInfo;

			writeDescSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescSet[1].descriptorCount = 1;
			writeDescSet[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescSet[1].dstArrayElement = 0;
			writeDescSet[1].dstBinding = 1;
			writeDescSet[1].dstSet = set[i];
			writeDescSet[1].pBufferInfo = &lightInfo;

			vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writeDescSet.size()), writeDescSet.data(), 0, nullptr);
		}

		return set;
	}

	size_t Renderer::GetNumActors()
	{
		return m_actors.size();
	}

	void Renderer::SetTerrain(Terrain* terrain)
	{
		if (m_terrain != nullptr) delete m_terrain;
		m_terrain = terrain;
	}

	void Renderer::SetCameraMatrix(Matrix4x4f const& cameraMatrix)
	{
		m_mvp.view = cameraMatrix;
	}


	uint32_t Renderer::FindSuitableMemType(uint32_t filter, VkMemoryPropertyFlags properties)
	{
		for (uint32_t i = 0; i < m_physicalDeviceMemProp.memoryTypeCount; i++)
		{
			if ((filter & (1 << i) && ((m_physicalDeviceMemProp.memoryTypes[i].propertyFlags & properties) == properties)))
			{
				return i;
			}
		}

		std::cerr << "Fail to find suitable memory type!" << std::endl;
		throw std::runtime_error("");
	}


	VkVertexInputBindingDescription GetVertexBindingDesc()
	{
		VkVertexInputBindingDescription desc = {};
		desc.binding = 0;
		desc.stride = sizeof(Vertex);
		desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return desc;
	}

	std::vector<VkVertexInputAttributeDescription> GetVertexAttriDesc()
	{
		std::vector<VkVertexInputAttributeDescription> arr;
		arr.resize(6);

		arr[0].binding = 0;
		arr[0].location = 0;
		arr[0].offset = offsetof(Vertex, position);
		arr[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;


		arr[1].binding = 0;
		arr[1].location = 1;
		arr[1].offset = offsetof(Vertex, color);
		arr[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;

		arr[2].binding = 0;
		arr[2].location = 2;
		arr[2].offset = offsetof(Vertex, texCoord);
		arr[2].format = VK_FORMAT_R32G32B32_SFLOAT;

		arr[3].binding = 0;
		arr[3].location = 3;
		arr[3].offset = offsetof(Vertex, normal);
		arr[3].format = VK_FORMAT_R32G32B32_SFLOAT;

		arr[4].binding = 0;
		arr[4].location = 4;
		arr[4].offset = offsetof(Vertex, boneWeights);
		arr[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;

		arr[5].binding = 0;
		arr[5].location = 5;
		arr[5].offset = offsetof(Vertex, affectedBonesID);
		arr[5].format = VK_FORMAT_R32G32B32A32_UINT;

		return arr;
	}
}