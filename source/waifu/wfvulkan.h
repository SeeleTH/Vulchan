#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include "vulkan.h"

namespace wfVulkan
{
#define VK_EXPORTED_FUNCTION(fun) extern PFN_##fun fun;
#include "wfvulkanfunctionlist.h"

	bool LoadVulkanLib();
}