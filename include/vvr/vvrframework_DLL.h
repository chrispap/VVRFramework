#ifdef _WIN32
#	ifdef VVRFramework_EXPORTS
#		define VVRFramework_API __declspec(dllexport)
#	else
#		define VVRFramework_API  __declspec(dllimport)
#	endif
#else
#	define VVRFramework_API
#endif
#define VVR_ENABLE_MODERN_GL 1
