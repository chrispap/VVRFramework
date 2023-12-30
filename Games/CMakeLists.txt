option(HIDE_CONSOLE_WINDOW "Show only gui window and not the the console" OFF)
link_directories(${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

file(GLOB TAVLI_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}
    tavli.cpp			tavli.h
)

file(GLOB ROLLINGCAR_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}
    RollingCar.cpp
)

add_executable(Tavli MACOSX_BUNDLE ${TAVLI_SRC_FILES})
target_link_libraries(Tavli
  optimized VVRFramework debug VVRFramework_d
  optimized GeoLib debug GeoLib_d
  optimized MathGeoLib debug MathGeoLib_d
  jsoncpp
)

add_executable(RollingCar MACOSX_BUNDLE ${ROLLINGCAR_SRC_FILES})
target_link_libraries(RollingCar
  optimized VVRFramework debug VVRFramework_d
  optimized GeoLib debug GeoLib_d
  optimized MathGeoLib debug MathGeoLib_d
  jsoncpp
)

set_property(TARGET Tavli PROPERTY FOLDER "Apps")

if(HIDE_CONSOLE_WINDOW AND WIN32 AND MSVC)
  set_target_properties(Tavli       PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  set_target_properties(RollingCar  PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/MathGeoLib/src)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/jsoncpp/include)
