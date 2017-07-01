cmake_minimum_required(VERSION 2.8.11)
project(VVRFramework)

### Build subdirs #######################################################################
add_subdirectory(3rdParty/GeoLib)
add_subdirectory(3rdParty/MathGeoLib)
add_subdirectory(src)
add_subdirectory(Examples)
#########################################################################################

#########################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET vvrframework PROPERTY FOLDER "Libraries")
set_property(TARGET GeoLib PROPERTY FOLDER "Libraries")
set_property(TARGET MathGeoLib PROPERTY FOLDER "Libraries")
#########################################################################################

#########################################################################################
set_target_properties(vvrframework PROPERTIES DEBUG_POSTFIX _d)
set_target_properties(MathGeoLib PROPERTIES DEBUG_POSTFIX _d)
set_target_properties(GeoLib PROPERTIES DEBUG_POSTFIX _d)
#########################################################################################

### INSTALL #############################################################################
install (DIRECTORY "GeoLib" DESTINATION "include" FILES_MATCHING PATTERN "*.h")
install (DIRECTORY "src" DESTINATION "include" FILES_MATCHING PATTERN "*.h")

### msvc runtime 
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "lib")

INCLUDE(CPack)
#########################################################################################
