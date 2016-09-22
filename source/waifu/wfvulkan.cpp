#include "wfvulkan.h"
#include "wfassert.h"

namespace wfVulkan
{
#define VK_EXPORTED_FUNCTION(fun) PFN_##fun fun = NULL;
#include "wfvulkanfunctionlist.h"

	bool LoadVulkanLib()
	{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		HMODULE vulkanLib = LoadLibrary("vulkan-1.dll");
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
		HMODULE vulkanLib = dlopen("libvulkan.so", RTLD_NOW);
#endif
		WFASSERT_MSG(vulkanLib != nullptr, Cannot load Vulkan Library);
		return true;
	}
}