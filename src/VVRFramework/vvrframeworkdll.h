#ifdef _WIN32
#	ifdef vvrframework_EXPORTS
#		define vvrframework_API __declspec(dllexport)
#	else
#		define vvrframework_API  __declspec(dllimport)
#	endif
#else
#	define vvrframework_API
#endif // WIN32
