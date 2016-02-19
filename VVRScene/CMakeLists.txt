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
GET_PROPERTY(prop TARGET Qt5::Core PROPERTY LOCATION)
GET_FILENAME_COMPONENT(QT_BIN_DIR ${prop} DIRECTORY)
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
if (WIN32)
  target_link_libraries(VVRScene GeoLib MathGeoLib ${Boost_LIBRARIES})
else ()
  target_link_libraries(VVRScene GeoLib ${Boost_LIBRARIES})
endif ()
#########################################################################################

#########################################################################################
install (TARGETS VVRScene DESTINATION "lib")

if(MSVC)
  ADD_CUSTOM_COMMAND(TARGET VVRScene POST_BUILD COMMAND
    ${QT_BIN_DIR}/windeployqt.exe $<TARGET_FILE:VVRScene> -dir ${CMAKE_CURRENT_BINARY_DIR}/lib)

  INSTALL (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib DESTINATION .)
endif(MSVC)

#########################################################################################
