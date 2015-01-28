#ifdef _WIN32
#	ifdef vvrscene_EXPORTS
#		define vvrscene_API __declspec(dllexport)
#       include <cstdlib>
#	else
#		define vvrscene_API  __declspec(dllimport)
#	endif
#else
#	define vvrscene_API
#   include <cstdlib>
#endif // WIN32
