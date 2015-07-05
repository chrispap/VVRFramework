#ifdef _WIN32
#	ifdef VVRPhysics_EXPORTS
#		define VVRPhysics_API __declspec(dllexport)
#	else
#		define VVRPhysics_API  __declspec(dllimport)
#	endif
#else
#	define VVRPhysics_API
#endif // WIN32
