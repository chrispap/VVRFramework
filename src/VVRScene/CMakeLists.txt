file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})

add_library(vvrscene SHARED ${SOURCE})

### QT STUFF #############################################################################
find_package(Qt5OpenGL REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
##########################################################################################

### QT Used modules ######################################################################
qt5_use_modules(vvrscene OpenGL)
target_link_libraries(vvrscene Qt5::OpenGL)
##########################################################################################

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(vvrscene ${OPENGL_LIBRARIES} )
