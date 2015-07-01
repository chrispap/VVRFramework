### Specify Executables ##################################################################
file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
add_executable(TestApp MACOSX_BUNDLE ${SOURCE})
set_property(TARGET TestApp PROPERTY FOLDER "Applications")
##########################################################################################

### Set include path #####################################################################
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/MathGeoLib/src)
include_directories(${CMAKE_SOURCE_DIR}/VVRScene)
include_directories(${Boost_INCLUDE_DIRS})
##########################################################################################

### Specify needed libs ##################################################################
target_link_libraries(TestApp VVRScene MathGeoLib GeoLib)
##########################################################################################

##########################################################################################
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
##########################################################################################
