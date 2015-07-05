#########################################################################################
file(GLOB SOURCE_FILES *.h *.cpp)
find_package(OpenGL REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE ${SOURCE_FILES})
add_library(VVRPhysics SHARED ${SOURCE} ${UI_FILES})#########################################################################################

#########################################################################################
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/GeoLib)
include_directories(${Boost_INCLUDE_DIRS})
#########################################################################################

#########################################################################################
target_link_libraries(VVRPhysics VVRScene)
#########################################################################################

### INSTALL #############################################################################
install (TARGETS VVRPhysics DESTINATION "lib")
install (DIRECTORY "./" DESTINATION "include/VVRPhysics" FILES_MATCHING PATTERN "*.h")
#########################################################################################
