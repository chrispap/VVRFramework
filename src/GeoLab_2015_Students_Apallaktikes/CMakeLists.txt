### Specify Executables ##################################################################
get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(SELF_DIR "${SELF_DIR}" NAME)
file(GLOB APP_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach( srcFile ${APP_SRC_FILES} )
    string( REPLACE ".cpp" "" appName ${srcFile} )
	if(EXISTS ${appName}.h)
		add_executable( ${appName} MACOSX_BUNDLE ${appName}.cpp ${appName}.h )
	else()
		add_executable( ${appName} MACOSX_BUNDLE ${appName}.cpp)
	endif()
    set_property(TARGET ${appName} PROPERTY FOLDER ${SELF_DIR})
endforeach( srcFile ${APP_SRC_FILES} )
##########################################################################################

### Set include path #####################################################################
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/GeoLib)
include_directories(${CMAKE_SOURCE_DIR}/MathGeoLib/src)
include_directories(${CMAKE_SOURCE_DIR}/VVRScene)
##########################################################################################

### Specify needed libs ##################################################################
foreach( srcFile ${APP_SRC_FILES} )
    string( REPLACE ".cpp" "" appName ${srcFile} )
    target_link_libraries(${appName} VVRScene MathGeoLib GeoLib)
endforeach( srcFile ${APP_SRC_FILES} )
##########################################################################################

##########################################################################################
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE PATH "")
##########################################################################################
