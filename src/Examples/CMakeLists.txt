### Build subdirs. Also add them to include path #########################################
include_directories(../VVRScene)
include_directories(../VVRPhysics)
##########################################################################################

### Executable ###########################################################################
set(TARGET vvrexampleapp CACHE TYPE STRING)
file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
##########################################################################################

### Find MathGeoLib ######################################################################
set(MathGeoLib_DIR CACHE PATH "Installation path of MathGeoLib")
include_directories(${MathGeoLib_DIR}/include)
link_directories(${MathGeoLib_DIR}/lib)
##########################################################################################

##########################################################################################
add_executable(${TARGET} ${SOURCE})
##########################################################################################

### Specify needed libs ##################################################################
target_link_libraries(${TARGET} VVRPhysics VVRScene MathGeoLib)
##########################################################################################

##########################################################################################
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
##########################################################################################
