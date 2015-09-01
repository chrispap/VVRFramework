### QT ##################################################################################
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
set(QRCS icons.qrc)
qt5_wrap_ui(UI_FILES window.ui)
qt5_add_resources(RCS ${QRCS})
#########################################################################################

#########################################################################################
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/MathGeoLib/src)
include_directories(${Boost_INCLUDE_DIRS})
#########################################################################################

#########################################################################################
add_library(VVRScene SHARED ${SOURCE} ${RCS} ${UI_FILES})
qt5_use_modules(VVRScene Widgets OpenGL)
target_link_libraries(VVRScene ${OPENGL_LIBRARIES} Qt5::Widgets Qt5::OpenGL)
target_link_libraries(VVRScene GeoLib ${Boost_LIBRARIES})
#########################################################################################

#########################################################################################
install (TARGETS VVRScene DESTINATION "lib")
#########################################################################################
