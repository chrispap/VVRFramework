### Specify Executables ##################################################################
file(GLOB APP_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach( srcFile ${APP_SRC_FILES} )
    string( REPLACE ".cpp" "" appName ${srcFile} )
    add_executable( ${appName} MACOSX_BUNDLE ${appName}.cpp ${appName}.h )
    set_property(TARGET ${appName} PROPERTY FOLDER "VVRExampleApps")
endforeach( srcFile ${APP_SRC_FILES} )
##########################################################################################

### Set include path #####################################################################
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/MathGeoLib/src)
include_directories(${CMAKE_SOURCE_DIR}/VVRScene)
include_directories(${CMAKE_SOURCE_DIR}/VVRPhysics)
##########################################################################################

### Specify needed libs ##################################################################
foreach( srcFile ${APP_SRC_FILES} )
    string( REPLACE ".cpp" "" appName ${srcFile} )
    target_link_libraries(${appName} VVRPhysics VVRScene GeoLib MathGeoLib)
endforeach( srcFile ${APP_SRC_FILES} )
##########################################################################################

##########################################################################################
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
##########################################################################################
