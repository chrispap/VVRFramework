### Executable ###########################################################################
set(TARGET VVRExampleApplication CACHE TYPE STRING)
file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
##########################################################################################

### Find MathGeoLib ######################################################################
set(MathGeoLib_DIR CACHE PATH "Installation path of MathGeoLib")
include_directories(${MathGeoLib_DIR}/include)
link_directories(${MathGeoLib_DIR}/lib)
##########################################################################################

##########################################################################################
add_executable(${TARGET} MACOSX_BUNDLE ${SOURCE})
##########################################################################################

### Set include path #####################################################################
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/VVRScene)
include_directories(${CMAKE_SOURCE_DIR}/VVRPhysics)
##########################################################################################

### Specify needed libs ##################################################################
target_link_libraries(${TARGET} VVRPhysics VVRScene MathGeoLib GeoLib)
##########################################################################################

##########################################################################################
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
##########################################################################################
