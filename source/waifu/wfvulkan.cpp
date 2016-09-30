#include "wfvulkan.h"
#include "wfassert.h"

#include <vector>

namespace wfVulkan
{
#define VK_EXPORTED_FUNCTION(fun) PFN_##fun fun = NULL;
#define VK_GLOBAL_LEVEL_FUNCTION(fun) PFN_##fun fun = NULL;
#define VK_INSTANCE_LEVEL_FUNCTION(fun) PFN_##fun fun = NULL;
#define VK_DEVICE_LEVEL_FUNCTION(fun) PFN_##fun fun = NULL;
#include "wfvulkanfunctionlist.h"

	VulkanBase::VulkanBase()
		: m_vulkanLib(nullptr)
		, m_winContext()
		, m_vulkanContext()
	{
	}

	VulkanBase::~VulkanBase()
	{
		if (m_vulkanLib)
		{
#if defined(OS_WIN32)
			FreeLibrary(m_vulkanLib);
#elif defined(OS_X11)
			dlclose(m_vulkanLib);
#endif
		}
	}

	bool VulkanBase::PrepareVulkan()
	{
		if (!loadVulkanLibrary())
			return false;
		if (!loadExportedEntryPoints())
			return false;
		if (!loadGlobalLevelEntryPoints())
			return false;
		if (!createInstance())
			return false;
		if (!loadInstanceLevelEntryPoints())
			return false;
		if (!createDevice())
			return false;
		return true;
	}

	bool VulkanBase::loadVulkanLibrary()
	{
		if (m_vulkanLib)
			return true;
#if defined(OS_WIN32)
		m_vulkanLib = LoadLibrary("vulkan-1.dll");
#elif defined(OS_X11)
		m_vkLib = dlopen("libvulkan.so", RTLD_NOW);
#endif
		WFASSERT_MSG(m_vulkanLib != nullptr, Cannot load Vulkan Library);
		WFDEB_COUT("Vulkan Library Loaded");
		return true;
	}

	bool VulkanBase::loadExportedEntryPoints()
	{
#if defined(OS_WIN32)
#define LoadProcAddress GetProcAddress
#elif defined(OS_X11)
#define LoadProcAddress dlsym
#endif
#define VK_EXPORTED_FUNCTION( fun ) \
if( !(fun = (PFN_##fun)LoadProcAddress( m_vulkanLib, #fun))) \
{ \
	WFASSERT_MSG(fun != nullptr, Cannot load exported entry point of #fun); \
	return false; \
}
#include "wfvulkanfunctionlist.h"
		WFDEB_COUT("Vulkan Exported Entrypoints Loaded");
		return true;
	}

	bool VulkanBase::loadGlobalLevelEntryPoints()
	{
#define VK_GLOBAL_LEVEL_FUNCTION( fun ) \
if( !(fun = (PFN_##fun)vkGetInstanceProcAddr( nullptr, #fun))) \
{ \
	WFASSERT_MSG(fun != nullptr, Cannot load global level entry point of #fun); \
	return false; \
}
#include "wfvulkanfunctionlist.h"
		WFDEB_COUT("Vulkan Global Level Entrypoints Loaded");
		return true;
	}

	bool VulkanBase::createInstance()
	{
		VkApplicationInfo appInfo = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"Date with Vulchan",
			VK_MAKE_VERSION(1, 0, 0),
			"Waifu",
			VK_MAKE_VERSION(1, 0, 0),
			VK_MAKE_VERSION(1, 0, 0)
		};

		VkInstanceCreateInfo instCreateInfo = {
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&appInfo,
			0,
			nullptr,
			0,
			nullptr
		};

		if (vkCreateInstance(&instCreateInfo, nullptr, &m_vulkanContext.Instance) != VK_SUCCESS)
		{
			WFASSERT_MSG(m_vulkanContext.Instance != nullptr, Cannot create instance);
			return false;
		}

		WFDEB_COUT("Vulkan Instance Created");
		return true;
	}
	bool VulkanBase::loadInstanceLevelEntryPoints()
	{
#define VK_INSTANCE_LEVEL_FUNCTION( fun ) \
if( !(fun = (PFN_##fun)vkGetInstanceProcAddr( m_vulkanContext.Instance, #fun))) \
{ \
	WFASSERT_MSG(fun != nullptr, Cannot load instance level entry point of #fun); \
	return false; \
}
#include "wfvulkanfunctionlist.h"
		WFDEB_COUT("Vulkan Instance Level Entrypoints Loaded");
		return true;
	}
	bool VulkanBase::createDevice()
	{
		uint32_t num_devices = 0;
		if ((vkEnumeratePhysicalDevices(m_vulkanContext.Instance, &num_devices, nullptr) != VK_SUCCESS) || (num_devices == 0))
		{
			WFASSERT_MSG(false, No available Vulkan enabled physical device);
			return false;
		}
		WFDEB_COUT("Vulkan Physical Devices: " << num_devices);
		std::vector<VkPhysicalDevice> physical_devices(num_devices);
		if (vkEnumeratePhysicalDevices(m_vulkanContext.Instance, &num_devices, &physical_devices[0]) != VK_SUCCESS)
		{
			WFASSERT_MSG(false, No available Vulkan enabled physical device);
			return false;
		}

		VkPhysicalDevice selected_physical_device = VK_NULL_HANDLE;
		uint32_t selected_queue_family_index = UINT32_MAX;
		for (uint32_t i = 0; i < num_devices; i++)
		{
			if (checkPhysicalDeviceProperties(physical_devices[i], selected_queue_family_index))
			{
				selected_physical_device = physical_devices[i];
				WFDEB_COUT(i << " was selected");
			}
		}

		return true;
	}
	bool VulkanBase::checkPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, uint32_t & graphicsQueueFamilyIndex)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		uint32_t majorVersion = VK_VERSION_MAJOR(deviceProperties.apiVersion);
		uint32_t minorVersion = VK_VERSION_MINOR(deviceProperties.apiVersion);
		uint32_t patchVersion = VK_VERSION_PATCH(deviceProperties.apiVersion);

		WFDEB_COUT("Name :\t" << deviceProperties.deviceName);
		WFDEB_COUT("Api Version :\t" << majorVersion << "," << minorVersion << "," << patchVersion);

		if (majorVersion < 1 && deviceProperties.limits.maxImageDimension2D < 4096)
		{
			return false;
		}

		return true;
	}
}