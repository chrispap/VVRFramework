cmake_minimum_required(VERSION 2.8.11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(VVRFramework)

### Build subdirs ########################################################################
add_subdirectory(GeoLib)
add_subdirectory(VVRScene)
add_subdirectory(VVRPhysics)
add_subdirectory(VVRExamples)
##########################################################################################
