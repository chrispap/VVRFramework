### QT STUFF #############################################################################
set(CMAKE_LIBRARY_PATH CACHE PATH "C:\\Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86")
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
##########################################################################################

##########################################################################################
file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
##########################################################################################

##########################################################################################
qt5_wrap_ui(UI_FILES window.ui)
##########################################################################################

### Find MathGeoLib ######################################################################
set(MathGeoLib_DIR CACHE PATH "Installation path of MathGeoLib")
include_directories(${MathGeoLib_DIR}/include)
link_directories(${MathGeoLib_DIR}/lib)
##########################################################################################

##########################################################################################
add_library(VVRScene SHARED ${SOURCE} ${UI_FILES})
##########################################################################################

### QT Used modules ######################################################################
qt5_use_modules(VVRScene Widgets OpenGL)
target_link_libraries(VVRScene Qt5::Widgets Qt5::OpenGL)
##########################################################################################

##########################################################################################
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(VVRScene ${OPENGL_LIBRARIES} )
##########################################################################################

### Link to MathGeolib ###################################################################
target_link_libraries(VVRScene PRIVATE MathGeoLib)
##########################################################################################
