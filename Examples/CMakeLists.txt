option(HIDE_CONSOLE_WINDOW "Show only gui window and not the the console" OFF)
link_directories(${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(GLOB APP_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

### Specify Executables #################################################################
foreach(srcFile ${APP_SRC_FILES})
  string( REPLACE ".cpp" "" appName ${srcFile})
  add_executable( ${appName} MACOSX_BUNDLE ${appName}.cpp ${appName}.h)
  target_link_libraries(${appName} 
    optimized vvrframework debug vvrframework_d
    optimized GeoLib debug GeoLib_d
    optimized MathGeoLib debug MathGeoLib_d)
  set_property(TARGET ${appName} PROPERTY FOLDER "Examples")
  if(HIDE_CONSOLE_WINDOW AND WIN32 AND MSVC) 
    set_target_properties(${appName} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  endif()
endforeach(srcFile ${APP_SRC_FILES})
#########################################################################################

### Set include path ####################################################################
include_directories(${CMAKE_SOURCE_DIR}/Src)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty//MathGeoLib/src)
#########################################################################################
