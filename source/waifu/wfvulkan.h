#pragma once

#include "wfmacro.h"
#include "wfos.h"

#if defined(OS_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#define VK_NO_PROTOTYPES
#include "vulkan.h"

namespace wfVulkan
{
#define VK_EXPORTED_FUNCTION(fun) extern PFN_##fun fun;
#define VK_GLOBAL_LEVEL_FUNCTION(fun) extern PFN_##fun fun;
#define VK_INSTANCE_LEVEL_FUNCTION(fun) extern PFN_##fun fun;
#define VK_DEVICE_LEVEL_FUNCTION(fun) extern PFN_##fun fun;

#include "wfvulkanfunctionlist.h"

	struct VulkanContext
	{
		VkInstance Instance;

		VulkanContext()
			: Instance(VK_NULL_HANDLE)
		{
		}
	};

	class VulkanBase
	{
	public:
		VulkanBase();
		~VulkanBase();

	private:
		bool loadVulkanLibrary();
		bool loadExportedEntryPoints();
		bool loadGlobalLevelEntryPoints();
		bool loadInstanceLevelEntryPoints();
		bool loadDeviceLevelEntryPoints();

		wfOS::LibraryHandle m_vulkanLib;
		wfOS::WindowContext m_winContext;
		VulkanContext m_vulkanContext;
	};
}