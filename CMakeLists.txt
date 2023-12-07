cmake_minimum_required(VERSION 3.22)
project(VVRFramework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

### Target Locations ####################################################################
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#########################################################################################

### Build subdirs #######################################################################
add_subdirectory(3rdParty/GeoLib)
add_subdirectory(3rdParty/MathGeoLib)
add_subdirectory(3rdParty/jsoncpp)
add_subdirectory(Core)
add_subdirectory(GeoLab)
add_subdirectory(Apps)
add_subdirectory(Games)
add_subdirectory(AllDemo)
add_subdirectory(NdiViewer)
#########################################################################################

#### Group (Visual Studio only) #########################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET VVRFramework PROPERTY FOLDER "Libraries")
set_property(TARGET GeoLib PROPERTY FOLDER "Libraries")
set_property(TARGET MathGeoLib PROPERTY FOLDER "Libraries")
#########################################################################################

#########################################################################################
set_target_properties(VVRFramework PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(MathGeoLib PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(GeoLib PROPERTIES DEBUG_POSTFIX "_d")
#########################################################################################

### INSTALL #############################################################################
install (DIRECTORY "3rdParty/GeoLib" DESTINATION "include" FILES_MATCHING PATTERN "*.h")
install (DIRECTORY "include" DESTINATION "./" FILES_MATCHING PATTERN "*.h")
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "lib")
INCLUDE(CPack)
#########################################################################################
