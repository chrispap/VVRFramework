file(GLOB SOURCE_FILES *.h *.cpp)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE ${SOURCE_FILES})
add_library(GeoLib SHARED ${SOURCE} )

### Link to Libraries ###################################################################
install (TARGETS GeoLib DESTINATION "lib")
install (DESTINATION "include/GeoLib" FILES_MATCHING PATTERN "*.h")
#########################################################################################
