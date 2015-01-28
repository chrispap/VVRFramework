### QT STUFF #############################################################################
set(CMAKE_LIBRARY_PATH CACHE PATH "C:\\Program Files (x86)\\Windows Kits\\8.1\\Lib\\winv6.3\\um\\x86")
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
##########################################################################################

### Build subdirs. Also add them to include path #########################################
include_directories(../VVRScene)
##########################################################################################

### Executable ###########################################################################
set(TARGET vvrtemplate CACHE TYPE STRING)
file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
##########################################################################################

##########################################################################################
qt5_wrap_ui(UI_FILES mainwindow.ui)
##########################################################################################

##########################################################################################
add_executable(${TARGET} ${SOURCE} ${UI_FILES})
##########################################################################################

### Specify needed libs ##################################################################
target_link_libraries(${TARGET} vvrscene)
target_link_libraries(${TARGET} MathGeoLib)
##########################################################################################

### QT Used modules ######################################################################
qt5_use_modules(${TARGET} Widgets OpenGL Network Concurrent)
target_link_libraries(${TARGET} Qt5::Widgets Qt5::OpenGL Qt5::Network Qt5::Concurrent)
##########################################################################################

##########################################################################################
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
##########################################################################################
