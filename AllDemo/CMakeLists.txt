### QT ###################################################################################
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui OpenGL OpenGLWidgets)
qt6_wrap_ui(UI_FILES demo_window.ui)
##########################################################################################

add_definitions(-DALL_DEMO_APP)

option(HIDE_CONSOLE_WINDOW "Show only gui window and not the the console" OFF)
link_directories(${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(GLOB APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}
    demo_window.cpp
)

add_executable(AllDemo MACOSX_BUNDLE ${APP_SRC_FILES} ${UI_FILES})
target_link_libraries(AllDemo Qt6::Widgets Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets)
target_link_libraries(AllDemo
  optimized VVRFramework debug VVRFramework_d
  optimized GeoLib debug GeoLib_d
  optimized MathGeoLib debug MathGeoLib_d
  jsoncpp)
set_property(TARGET AllDemo PROPERTY FOLDER "Apps")
if(HIDE_CONSOLE_WINDOW AND WIN32 AND MSVC)
  set_target_properties(AllDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/MathGeoLib/src)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/jsoncpp/include)
include_directories(${CMAKE_SOURCE_DIR})
##########################################################################################
