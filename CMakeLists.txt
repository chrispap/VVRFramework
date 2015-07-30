cmake_minimum_required(VERSION 2.8.11)
project(VVRFramework)

### Find Boost ##########################################################################
set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_NEEDED_LIBS log log_setup system filesystem date_time thread chrono regex)
find_package(Boost 1.57.0 COMPONENTS ${Boost_NEEDED_LIBS})
#########################################################################################

### Build subdirs #######################################################################
add_subdirectory(GLFW)
add_subdirectory(GeoLib)
add_subdirectory(MathGeoLib)
add_subdirectory(VVRScene)
#########################################################################################

#########################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET VVRScene PROPERTY FOLDER "VVRLibraries")
set_property(TARGET GeoLib PROPERTY FOLDER "VVRLibraries")
set_property(TARGET MathGeoLib PROPERTY FOLDER "VVRLibraries")
set_property(TARGET glfw PROPERTY FOLDER "VVRLibraries")
#########################################################################################

#########################################################################################
set_target_properties(VVRScene PROPERTIES DEBUG_POSTFIX _d)
set_target_properties(MathGeoLib PROPERTIES DEBUG_POSTFIX _d)
set_target_properties(GeoLib PROPERTIES DEBUG_POSTFIX _d)
set_target_properties(glfw PROPERTIES DEBUG_POSTFIX _d)
#########################################################################################

### INSTALL #############################################################################
option(COPY_BOOST_HEADERS "Copy Boost headers to installation dir" OFF)
# Gather and install dependencies
FOREACH(module ${Boost_NEEDED_LIBS})
	STRING(TOUPPER ${module} module_u)
	SET(lib_name_r Boost_${module_u}_LIBRARY_RELEASE)
	SET(lib_name_d Boost_${module_u}_LIBRARY_DEBUG)
	STRING(REPLACE ".lib" ".dll" lib_r_dlls ${${lib_name_r}})
	STRING(REPLACE ".lib" ".dll" lib_d_dlls ${${lib_name_d}})
	SET(deps ${deps} ${lib_r_dlls} ${lib_d_dlls} )
ENDFOREACH(module ${Boost_NEEDED_LIBS})

IF(${COPY_BOOST_HEADERS}) 
	install (DIRECTORY ${Boost_INCLUDE_DIR}/boost DESTINATION "include/")
ENDIF()

#Install headers
install (DIRECTORY "GeoLib" DESTINATION "include/GeoLib" FILES_MATCHING PATTERN "*.h")
install (DIRECTORY "VVRScene" DESTINATION "include/VVRScene" FILES_MATCHING PATTERN "*.h")
install (FILES ${deps} DESTINATION "lib")

INCLUDE(CPack)
#########################################################################################
