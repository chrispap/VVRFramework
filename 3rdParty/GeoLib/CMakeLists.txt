file(GLOB SOURCE_FILES *.h *.cpp)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE ${SOURCE_FILES})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_library(GeoLib SHARED ${SOURCE})
install (TARGETS GeoLib DESTINATION "lib")
