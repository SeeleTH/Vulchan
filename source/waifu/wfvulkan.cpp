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
		if (m_vulkanLib)
		{
#if defined(OS_WIN32)
			FreeLibrary(m_vulkanLib);
#elif defined(OS_X11)
			dlclose(m_vulkanLib);
#endif
		}
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
		return true;
	}
}