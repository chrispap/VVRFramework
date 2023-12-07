add_definitions(-D__BASE_DIR__="${PROJECT_SOURCE_DIR}/")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

### QT ##################################################################################
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui OpenGL OpenGLWidgets)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(QRCS icons.qrc)
qt6_wrap_ui(UI_FILES window.ui)
qt6_add_resources(RCS ${QRCS})
GET_PROPERTY(prop TARGET Qt6::Core PROPERTY LOCATION)
GET_FILENAME_COMPONENT(QT_BIN_DIR ${prop} DIRECTORY)
#########################################################################################

### Sources #############################################################################
set(IMPL_FILES
  window.cpp window.h
  glwidget.cpp
  scene.cpp
  scene_modern.cpp
  drawing.cpp
  palette.cpp
  mesh.cpp
  kdtree.cpp
  utils.cpp
  settings.cpp
  dsp.cpp
  tiny_obj_loader.cpp tiny_obj_loader.h
  stdout_redirector.h
)
set(INCL_FILES
  ../include/vvr/macros.h
  ../include/vvr/glwidget.h
  ../include/vvr/scene.h
  ../include/vvr/scene_modern.h
  ../include/vvr/geom.h
  ../include/vvr/drawing.h
  ../include/vvr/palette.h
  ../include/vvr/dragging.h
  ../include/vvr/picking.h
  ../include/vvr/mesh.h
  ../include/vvr/kdtree.h
  ../include/vvr/bspline.h
  ../include/vvr/utils.h
  ../include/vvr/settings.h
  ../include/vvr/animation.h
  ../include/vvr/dsp.h
  ../include/vvr/command.h
  ../include/vvr/vvrframework_DLL.h
)
set(SOURCE ${IMPL_FILES} ${INCL_FILES})
#########################################################################################

#########################################################################################
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/MathGeoLib/src)
#########################################################################################

#########################################################################################
add_library(VVRFramework SHARED ${SOURCE} ${RCS} ${UI_FILES})
target_link_libraries(VVRFramework ${OPENGL_LIBRARIES} Qt6::Widgets Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets)
if (WIN32 OR APPLE)
  target_link_libraries(VVRFramework  GeoLib MathGeoLib)
elseif(UNIX)
  target_link_libraries(VVRFramework GeoLib)
endif ()
#########################################################################################

#########################################################################################
option(RUN_WINDEPLOYQT "Invoke the Qt provided script to gather all qt deps in the lib binary" OFF)
install (TARGETS VVRFramework DESTINATION "lib")
#install (FILES ${UI_FILES} DESTINATION "include/vvr" )
if(MSVC AND RUN_WINDEPLOYQT)
  ADD_CUSTOM_COMMAND(TARGET VVRFramework POST_BUILD COMMAND
    ${QT_BIN_DIR}/windeployqt.exe $<TARGET_FILE:VVRFramework> -dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
  INSTALL (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib DESTINATION .)
endif(MSVC AND RUN_WINDEPLOYQT)
#########################################################################################
