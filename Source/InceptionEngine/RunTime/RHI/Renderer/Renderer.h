#pragma once

#include "ThirdPartyLibrary/vulkan/Include/vulkan/vulkan.h"
#include "EngineGlobals/RenderGlobals.h"
#include "Vertex.h"
#include "ShaderPath.h"


#include <optional>
#include <mutex>


namespace inceptionengine
{

	class IRenderObject;

	class WindowHandler;

	class Terrain;

	VkVertexInputBindingDescription GetVertexBindingDesc();

	std::vector<VkVertexInputAttributeDescription> GetVertexAttriDesc();

	struct Light
	{
		Vec4f m_locationAndIntensity = Vec4f(0.0f);
	};


	struct MVP
	{
		Matrix4x4f view;
		Matrix4x4f proj;
	};


	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamilyIndex;
		std::optional<uint32_t> presentFamilyIndex;

		bool IsComplete()
		{
			return graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value();
		}
	};


	struct VertexBuffer
	{
		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	};

	struct IndexBuffer
	{
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
	};


	struct Pipeline
	{
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	};

	struct Texture
	{
		VkImage textureImage = VK_NULL_HANDLE;
		VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
	};


	struct CubeMap
	{
		VkImage cubeMapImage = VK_NULL_HANDLE;
		VkDeviceMemory cubeMapImageMemory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
	};



	//the transformation buffer, by design, is a vulkan uniform buffer of array of 200 Matrix4x4f
	//the first mat4 is always identity, for the usage of static mesh
	//the second mat4 is the model transformation matrix
	//the last 198 matrix are the bone transformation matrices

	struct UniformBuffer
	{
		std::array<VkBuffer, NumOfRenderBuffers> boneTransform = {};
		std::array<VkDeviceMemory, NumOfRenderBuffers> boneTransformMemory = {};

		std::array<VkBuffer, NumOfRenderBuffers> light = {};
		std::array<VkDeviceMemory, NumOfRenderBuffers> lightMemory = {};

	};

	//this struct wrap up the vkdescriptor creation
	//which tell the shader the layout of the data in the 
	//uniform buffer object
	struct UniformBufferDescription
	{
		VkDescriptorPool pool = VK_NULL_HANDLE;
		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		std::array<VkDescriptorSet, NumOfRenderBuffers> descriptorSet = {};
	};

	struct RenderUnit
	{
		VkBuffer* vertexBuffer = nullptr;
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		uint32_t numOfIndices = 0;
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkDescriptorSet* descriptorSet = nullptr;
	};

	using RenderObejctID = std::vector<IRenderObject>::size_type;

	class Renderer
	{
	
	public:
		static constexpr RenderObejctID InvalidRenderObjectID = std::numeric_limits<RenderObejctID>::max();

		Renderer() = default;

		~Renderer() = default;

		/*
		Disable copy and move
		*/
		Renderer(Renderer const&) = delete;

		Renderer& operator = (Renderer const&) = delete;

		Renderer(Renderer&&) = delete;

		Renderer& operator = (Renderer&&) = delete;

		/*
		These init and destroy functions are intended to used by engine who
		is in responsible for world beginning, ending, and drawing.
		*/
		void Initialize(WindowHandler& windowHandler);

		void Destroy();

		void DrawFrame();

		/*
		Render object's creation functions. These functions are intended
		to be used by RenderSystm in the ECS hierachy
		*/
		void CreateVertexBuffer(VertexBuffer& vertexBuffer, std::vector<Vertex> const& vertices);

		void DestroyVertexBuffer(VertexBuffer& vertexBuffer);

		void CreateIndexBuffer(IndexBuffer& indexBuffer, std::vector<uint32_t> const& indices);

		void DestroyIndexBuffer(IndexBuffer& indexBuffer);

		void CreateTexture(Texture& texture, std::string filePath);

		void DestroyTexture(Texture& texture);

		void CreateCubeMap(CubeMap& cubeMap, std::array<std::string, 6> const& texturePaths);

		void DestroyCubeMap(CubeMap& cubeMap);

		void CreateUniformBuffer(UniformBuffer& uBuffer);

		void DestroyUniformBuffer(UniformBuffer& uBuffer);

		void CreateUniformBufferDescription(UniformBufferDescription& uBufferDesc,
											UniformBuffer const& uBuffer,
											Texture const& texture);

		void DestroyUniformBufferDescription(UniformBufferDescription& desc);

		void CreatePipeline(Pipeline& pipeline,
							ShaderPath const& shaderpath,
							UniformBufferDescription& dataDesc,
							bool useDepthTest = true,
							VkPrimitiveTopology topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		void DestroyPipeline(Pipeline& pipeline);

		void CreateSkyboxDataDescription(UniformBufferDescription& desc, UniformBuffer const& uBuffer, CubeMap const& cubemap);

		/*
		Uniform buffer modification functions. Intend to use by Render Syetem to modify light
		and animation data in uniform buffer.
		*/
		void InitializeUniformBuffer(UniformBuffer& uBuffer,
									 const std::vector<Matrix4x4f>& transformations = { Matrix4x4f(1.0), Matrix4x4f(1.0) },
									 Light* light = nullptr);

		void InitializeLight(UniformBuffer& uBuffer, Light* light);

		void UpdateUniformBuffer(UniformBuffer& uBuffer, const std::vector<Matrix4x4f>& transformations, Light* light);

		RenderObejctID SubmitToDevice(IRenderObject* renderComponent);

		size_t GetNumActors();

		void SetTerrain(Terrain* terrain);

		void SetCameraMatirx(Matrix4x4f const& cameraMatrix);


	private:
		/*
		Internal implmentation of the interface
		*/
		void CreateInstance();

		void CreateSurface(WindowHandler& windowHandler);

		void CreatePhysicalDevice();

		bool CheckPhysicalDeviceExtensionSupport();

		void GetQueueFamilyIndices();

		void CreateLogicalDevice();

		void CreateSwapchain();

		void CreateSwapchainImageViews();

		void CreateDepthResources();

		void CreateRenderPass();

		void CreateFramebuffer();

		void CreateCommandPool();

		void CreateCommandBuffers();

		void CreateSemaphoreAndFence();

		std::vector<char> LoadShaderBinary(const std::string& filePath);

		VkShaderModule LoadShaderModule(const std::string& filePath);

		void CleaupSwapChain();

		uint32_t FindSuitableMemType(uint32_t filter, VkMemoryPropertyFlags properties);

		void CreateBuffer(VkBuffer& buffer,
						  VkBufferUsageFlags usageFlags,
						  VkDeviceSize size,
						  VkDeviceMemory& memory,
						  VkMemoryPropertyFlags memProperties);

		void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

		void RecordCommandBuffer(uint32_t i);

		void CreateImage(VkImage& image,
						 VkDeviceMemory& imageMem,
						 uint32_t width,
						 uint32_t height,
						 uint32_t depth,
						 uint32_t arrayLayers,
						 uint32_t mipLevels,
						 VkImageCreateFlags flags,
						 VkImageType type,
						 VkFormat format,
						 VkImageTiling tiling,
						 VkImageUsageFlags usage,
						 VkMemoryPropertyFlags memoryProperties);


		VkCommandBuffer BeginSingleTimeCommandBuffer();

		void EndSingleTimeCommandBuffer(VkCommandBuffer& buffer);

		void TransitImageLayout(VkImage& image,
								uint32_t numLayer,
								VkImageLayout oldLayout,
								VkImageLayout newLayout,
								VkAccessFlags srcAccessMask,
								VkAccessFlags dstAccessMask,
								VkImageAspectFlags aspectFlag,
								VkPipelineStageFlags srcStageMask,
								VkPipelineStageFlags dstStageMask);


		VkImageView CreateImageView(VkImage& image, uint32_t numLayers, VkImageAspectFlags aspectMask, VkFormat viewFormat, VkImageViewType viewType);

		VkSampler CreateTextureSampler();

		VkDescriptorSetLayout CreateSkyboxDescriptorSetLayout();

		std::array<VkDescriptorSet, NumOfRenderBuffers> CreateSkyboxDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool, const UniformBuffer& uBuffer, const CubeMap& cubemap);

		VkDescriptorPool CreateDescriptorPool();

		VkDescriptorSetLayout CreateDescriptorSetLayout();

		std::array<VkDescriptorSet, NumOfRenderBuffers> CreateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool, const UniformBuffer& uBuffer, const Texture& texture);

	private:
		/*
		const properties
		*/
		std::string const GraphicsQueue = "graphicsQueue";

		std::string const PresentQueue = "presentQueue";

		std::vector<const char*> const DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	private:
		/*
		vulkan objects
		*/
		VkInstance m_instance = VK_NULL_HANDLE;

		VkSurfaceKHR m_surface = VK_NULL_HANDLE;

		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemProp = {};

		QueueFamilyIndices m_queueFamilyIndices;

		std::unordered_map<std::string, VkQueue> m_queues;
	
		VkDevice m_device = VK_NULL_HANDLE;

		VkFormat m_imageFormat;

		VkExtent2D mSwapchainExtent;

		VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;

		std::vector<VkImage> mSwapchainImages;

		std::vector<VkImageView> mSwapchainImageViews;

		VkImage mDepthImage = VK_NULL_HANDLE;

		VkImageView mDepthImageView = VK_NULL_HANDLE;

		VkDeviceMemory mDepthImageMem = VK_NULL_HANDLE;

		VkCommandPool m_commandPool = VK_NULL_HANDLE;

		std::vector<VkCommandBuffer> m_commandBuffers;

		std::vector<VkFramebuffer> m_framebuffers;

		VkRenderPass m_renderPass = VK_NULL_HANDLE;

		std::vector<VkSemaphore> m_graphicsFinishSemaphores;

		std::vector<VkFence> m_imageFinishRenderFence;

		VkFence m_acquireImageFence = VK_NULL_HANDLE;

	private:
		/*
		Render-used objects
		*/
		uint32_t m_nextRenderImgIdx = 0;

		uint32_t m_windowWidth = 0;

		uint32_t m_windowHeight = 0;

		bool isChangingSize = false;

		bool m_minimized = false;

		MVP m_mvp = {};

		Terrain* m_terrain = nullptr;

		std::recursive_mutex m_actorLock;

		std::vector<IRenderObject*> m_actors;
	};


	/*
struct Image {};
struct CubeMap {};

template<typename T>
struct Texturee
{
	VkImage m_textureImage = VK_NULL_HANDLE;
	VkDeviceMemory m_textureImageMemory = VK_NULL_HANDLE;
	VkImageView m_view = VK_NULL_HANDLE;
	VkSampler m_sampler = VK_NULL_HANDLE;
};

template<>
struct Texturee<Image>;

template<>
struct Texturee<CubeMap>;*/
}
