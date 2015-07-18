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
add_subdirectory(VVRPhysics)
#########################################################################################

#########################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET VVRScene PROPERTY FOLDER "VVRLibraries")
set_property(TARGET VVRPhysics PROPERTY FOLDER "VVRLibraries")
set_property(TARGET GeoLib PROPERTY FOLDER "VVRLibraries")
set_property(TARGET MathGeoLib PROPERTY FOLDER "VVRLibraries")
#########################################################################################

INCLUDE(CPack)
