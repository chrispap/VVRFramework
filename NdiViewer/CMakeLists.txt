### NOTES:
### https://stackoverflow.com/questions/68036484/qt6-qt-qpa-plugin-could-not-load-the-qt-platform-plugin-xcb-in-even-thou
###

### QT ###################################################################################
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 COMPONENTS
  Widgets
  Gui
  Multimedia
  MultimediaWidgets
  Concurrent
  OpenGL
  OpenGLWidgets
REQUIRED)
qt6_wrap_ui(UI_FILES ndiviewer_window.ui)
##########################################################################################

### NDI #################################################################################
set(NDI_SDK_DIR CACHE PATH "Location of NDI SDK")
if (APPLE)
  include_directories(${NDI_SDK_DIR}/include)
  set(MY_LIBRARIES ${MY_LIBRARIES} ${NDI_SDK_DIR}/lib/macOS/libndi.dylib)
elseif (MSVC)
  include_directories(${NDI_SDK_DIR}/include)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MY_LIBRARIES ${MY_LIBRARIES} ${NDI_SDK_DIR}/Lib/x64/Processing.NDI.Lib.x64.lib)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(MY_LIBRARIES ${MY_LIBRARIES} ${NDI_SDK_DIR}/Lib/x86/Processing.NDI.Lib.x86.lib)
  endif()
elseif(UNIX)
  include_directories(${NDI_SDK_DIR}/include)
  set(MY_LIBRARIES ${MY_LIBRARIES} ${NDI_SDK_DIR}/lib/x86_64-linux-gnu/libndi.so)
endif(APPLE)
#########################################################################################

option(HIDE_CONSOLE_WINDOW "Show only gui window and not the the console" OFF)
link_directories(${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(GLOB APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}
    ndiviewer_window.cpp
)

add_executable(ndiviewer MACOSX_BUNDLE ${APP_SRC_FILES} ${UI_FILES})
target_link_libraries(ndiviewer
  Qt6::Widgets
  Qt6::Gui
  Qt6::Multimedia
  Qt6::MultimediaWidgets
  Qt6::Concurrent
  Qt6::OpenGL
  Qt6::OpenGLWidgets
)
target_link_libraries(ndiviewer
  optimized VVRFramework debug VVRFramework_d
  optimized GeoLib debug GeoLib_d
  optimized MathGeoLib debug MathGeoLib_d
  ${MY_LIBRARIES}
  jsoncpp)
set_property(TARGET ndiviewer PROPERTY FOLDER "Apps")
if(HIDE_CONSOLE_WINDOW AND WIN32 AND MSVC)
  set_target_properties(ndiviewer PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/MathGeoLib/src)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/jsoncpp/include)
include_directories(${CMAKE_SOURCE_DIR})
##########################################################################################
