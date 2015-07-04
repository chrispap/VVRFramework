### Set include path ####################################################################
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/MathGeoLib/src)
include_directories(${CMAKE_SOURCE_DIR}/VVRScene)
include_directories(${CMAKE_SOURCE_DIR}/tet-cpp-client/include)
include_directories(${Boost_INCLUDE_DIRS})
#########################################################################################

### Set library path ####################################################################
link_directories(${Boost_LIBRARY_DIRS})
#########################################################################################

### Specify Executables #################################################################
file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
add_executable(EyeTracking MACOSX_BUNDLE ${SOURCE})
set_property(TARGET EyeTracking PROPERTY FOLDER "Applications")
#########################################################################################

### Specify needed libs #################################################################
target_link_libraries(EyeTracking VVRScene MathGeoLib GeoLib GazeApiLib ${Boost_LIBRARIES})
#target_link_libraries(EyeTracking debug GazeApiLibD optimized GazeApiLib)
#########################################################################################
