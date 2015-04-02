cmake_minimum_required(VERSION 2.8)

project(GeoLib)

file(GLOB SOURCE_FILES *.h *.cpp)
set(SOURCE ${SOURCE_FILES})
add_library(GeoLib SHARED ${SOURCE} )
