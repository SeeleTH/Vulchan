#if !defined(VK_EXPORTED_FUNCTION)
#define VK_EXPORTED_FUNCTION( fun )
#endif

VK_EXPORTED_FUNCTION(vkGetInstanceProcAddr)

#undef VK_EXPORTED_FUNCTION

#if !defined(VK_GLOBAL_LEVEL_FUNCTION)
#define VK_GLOBAL_LEVEL_FUNCTION( fun )
#endif

VK_GLOBAL_LEVEL_FUNCTION(vkCreateInstance)
VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceExtensionProperties)
VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceLayerProperties)

#undef VK_GLOBAL_LEVEL_FUNCTION

#if !defined(VK_INSTANCE_LEVEL_FUNCTION)
#define VK_INSTANCE_LEVEL_FUNCTION( fun )
#endif

VK_INSTANCE_LEVEL_FUNCTION(vkEnumeratePhysicalDevices)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceProperties)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceFeatures)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
VK_INSTANCE_LEVEL_FUNCTION(vkCreateDevice)
VK_INSTANCE_LEVEL_FUNCTION(vkGetDeviceProcAddr)
VK_INSTANCE_LEVEL_FUNCTION(vkDestroyInstance)

#undef VK_INSTANCE_LEVEL_FUNCTION

#if !defined(VK_DEVICE_LEVEL_FUNCTION)
#define VK_DEVICE_LEVEL_FUNCTION( fun )
#endif

#undef VK_DEVICE_LEVEL_FUNCTION