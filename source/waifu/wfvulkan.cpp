#include "wfvulkan.h"
#include "wfassert.h"


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
		if (m_vulkanContext.Device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(m_vulkanContext.Device);
			vkDestroyDevice(m_vulkanContext.Device, nullptr);
		}

		if (m_vulkanContext.Instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(m_vulkanContext.Instance, nullptr);
		}

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
		if (!loadDeviceLevelEntryPoints())
			return false;
		if (!getDeviceQueues())
			return false;
		WFDEB_COUT("Vulkan was Initialized");
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
		uint32_t extensionCount = 0;
		if ((vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr) != VK_SUCCESS)
			|| extensionCount == 0)
		{
			WFDEB_COUT("Could not enumerate extension list");
			return false;
		}

		std::vector<VkExtensionProperties> availableExtension(extensionCount);
		if ((vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &availableExtension[0]) 
			!= VK_SUCCESS))
		{
			WFDEB_COUT("Could not enumerate extension list");
			return false;
		}

		std::vector<const char*> extensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XCB_KHR)
			VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLAYFORM_XLIB_KHR)
			VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
		};

		for (auto checkEx : extensions)
		{
			if (!checkExtensionAvailability(checkEx, availableExtension))
			{
				WFDEB_COUT("Extension missed: " << checkEx);
				return false;
			}
		}

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
			static_cast<uint32_t>(extensions.size()),
			&extensions[0]
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
		std::vector<VkPhysicalDevice> physicalDevices(num_devices);
		if (vkEnumeratePhysicalDevices(m_vulkanContext.Instance, &num_devices, &physicalDevices[0]) != VK_SUCCESS)
		{
			WFASSERT_MSG(false, No available Vulkan enabled physical device);
			return false;
		}

		VkPhysicalDevice selectedPhysicalDevice = VK_NULL_HANDLE;
		uint32_t selectedQueueFamilyIndex = UINT32_MAX;
		for (uint32_t i = 0; i < num_devices; i++)
		{
			if (checkPhysicalDeviceProperties(physicalDevices[i], selectedQueueFamilyIndex))
			{
				selectedPhysicalDevice = physicalDevices[i];
				WFDEB_COUT(i << " was selected");
			}
		}

		if (selectedPhysicalDevice == VK_NULL_HANDLE)
		{
			WFDEB_COUT("Cannot find compatible physical device");
			return false;
		}

		std::vector<float> queuePriorities = { 1.0f };

		VkDeviceQueueCreateInfo queueCreateInfo = {
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr,
			0,
			selectedQueueFamilyIndex,
			static_cast<uint32_t>(queuePriorities.size()),
			&queuePriorities[0]
		};

		VkDeviceCreateInfo deviceCreateInfo = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,
			1,
			&queueCreateInfo,
			0,
			nullptr,
			0,
			nullptr,
			nullptr
		};

		if (vkCreateDevice(selectedPhysicalDevice, &deviceCreateInfo, nullptr
			, &m_vulkanContext.Device) != VK_SUCCESS)
		{
			WFDEB_COUT("Could not create Vulkan device!");
			return false;
		}

		m_vulkanContext.GraphicsQueue.FamilyIndex = selectedQueueFamilyIndex;

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

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		if (queueFamilyCount == 0)
		{
			return false;
		}

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, &queueFamilyProperties[0]);
		uint32_t checkQueueInd = 0;
		for (checkQueueInd = 0; checkQueueInd < queueFamilyCount; checkQueueInd++)
		{
			if (queueFamilyProperties[checkQueueInd].queueCount > 0 &&
				queueFamilyProperties[checkQueueInd].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueFamilyIndex = checkQueueInd;
				break;
			}
		}
		if (checkQueueInd >= queueFamilyCount)
		{
			return false;
		}

		return true;
	}

	bool VulkanBase::loadDeviceLevelEntryPoints()
	{
#define VK_DEVICE_LEVEL_FUNCTION( fun ) \
if(!(fun = (PFN_##fun)vkGetDeviceProcAddr(m_vulkanContext.Device, #fun))) \
{ \
	WFASSERT_MSG(fun != nullptr, Cannot load device level entry point of #fun); \
	return false; \
}
#include "wfvulkanfunctionlist.h"
		return true;
	}

	bool VulkanBase::getDeviceQueues()
	{
		vkGetDeviceQueue(m_vulkanContext.Device, m_vulkanContext.GraphicsQueue.FamilyIndex
			, 0, &m_vulkanContext.GraphicsQueue.Handle);
		return true;
	}

	bool VulkanBase::checkExtensionAvailability(const char * extension, const std::vector<VkExtensionProperties>& extensionList)
	{
		for (auto availEx : extensionList)
		{
			if (strcmp(extension, availEx.extensionName) == 0)
				return true;
		}
		return false;
	}
}