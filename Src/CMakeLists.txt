add_definitions(-D__BASE_DIR__="${PROJECT_SOURCE_DIR}/")

### QT ##################################################################################
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(QRCS icons.qrc)
qt5_wrap_ui(UI_FILES window.ui)
qt5_add_resources(RCS ${QRCS})
GET_PROPERTY(prop TARGET Qt5::Core PROPERTY LOCATION)
GET_FILENAME_COMPONENT(QT_BIN_DIR ${prop} DIRECTORY)
#########################################################################################

### Sources #############################################################################
set(IMPL_FILES
  window.cpp window.h
  glwidget.cpp
  scene.cpp
  drawing.cpp
  mesh.cpp
  kdtree.cpp
  utils.cpp
  settings.cpp
  dsp.cpp
  tiny_obj_loader.cpp tiny_obj_loader.h
  stdout_redirector.h
)
set(INCL_FILES
  ../include/vvr/glwidget.h
  ../include/vvr/scene.h
  ../include/vvr/drawing.h
  ../include/vvr/mesh.h
  ../include/vvr/kdtree.h
  ../include/vvr/bspline.h
  ../include/vvr/utils.h
  ../include/vvr/settings.h
  ../include/vvr/dsp.h
  ../include/vvr/animation.h
  ../include/vvr/macros.h
  ../include/vvr/vvrframework_DLL.h
)
set(SOURCE ${IMPL_FILES} ${INCL_FILES})
#########################################################################################

#########################################################################################
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty//MathGeoLib/src)
#########################################################################################

#########################################################################################
add_library(vvrframework SHARED ${SOURCE} ${RCS} ${UI_FILES})
target_link_libraries(vvrframework ${OPENGL_LIBRARIES} Qt5::Widgets Qt5::OpenGL)
if (WIN32 OR APPLE)
  target_link_libraries(vvrframework  GeoLib MathGeoLib)
elseif(UNIX)
  target_link_libraries(vvrframework GeoLib)
endif ()
#########################################################################################

#########################################################################################
option(RUN_WINDEPLOYQT "Invoke the Qt provided script to gather all qt deps in the lib binary" OFF)
install (TARGETS vvrframework DESTINATION "lib")
#install (FILES ${UI_FILES} DESTINATION "include/vvr" )
if(MSVC AND RUN_WINDEPLOYQT)
  ADD_CUSTOM_COMMAND(TARGET vvrframework POST_BUILD COMMAND
    ${QT_BIN_DIR}/windeployqt.exe $<TARGET_FILE:vvrframework> -dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
  INSTALL (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib DESTINATION .)
endif(MSVC AND RUN_WINDEPLOYQT)
#########################################################################################
