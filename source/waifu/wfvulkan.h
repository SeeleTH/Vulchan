#pragma once

#include "wfmacro.h"
#include "wfos.h"

#if defined(OS_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#define VK_NO_PROTOTYPES
#include "vulkan.h"

#include <vector>

#define USE_SWAPCHAIN_EXTENSION

namespace wfVulkan
{
#define VK_EXPORTED_FUNCTION(fun) extern PFN_##fun fun;
#define VK_GLOBAL_LEVEL_FUNCTION(fun) extern PFN_##fun fun;
#define VK_INSTANCE_LEVEL_FUNCTION(fun) extern PFN_##fun fun;
#define VK_DEVICE_LEVEL_FUNCTION(fun) extern PFN_##fun fun;

#include "wfvulkanfunctionlist.h"

	struct QueueContext
	{
		VkQueue Handle;
		uint32_t FamilyIndex;
		QueueContext()
			: Handle(VK_NULL_HANDLE)
			, FamilyIndex(0)
		{}
	};

	struct VulkanContext
	{
		VkInstance Instance;
		VkDevice Device;
		QueueContext GraphicsQueue;
		QueueContext PresentQueue;

		VulkanContext()
			: Instance(VK_NULL_HANDLE)
			, Device(VK_NULL_HANDLE)
			, GraphicsQueue()
			, PresentQueue()
		{
		}
	};

	class VulkanBase
	{
	public:
		VulkanBase();
		~VulkanBase();

		bool PrepareVulkan();

	private:
		bool loadVulkanLibrary();
		bool loadExportedEntryPoints();
		bool loadGlobalLevelEntryPoints();
		bool createInstance();
		bool loadInstanceLevelEntryPoints();
		bool createDevice();
		bool checkPhysicalDeviceProperties(VkPhysicalDevice physicalDevice
			, uint32_t &graphicsQueueFamilyIndex);
		bool loadDeviceLevelEntryPoints();
		bool getDeviceQueues();

		bool checkExtensionAvailability(const char* extension
			, const std::vector<VkExtensionProperties> &extensionList);

		wfOS::LibraryHandle m_vulkanLib;
		wfOS::WindowContext m_winContext;
		VulkanContext m_vulkanContext;
	};
}