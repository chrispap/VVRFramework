cmake_minimum_required(VERSION 2.8.11)

project(VVRFramework)

### Find Boost ##########################################################################
set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.57.0 COMPONENTS 
    log log_setup system filesystem date_time thread chrono regex)
#########################################################################################

### Build subdirs #######################################################################
add_subdirectory(GeoLib)
add_subdirectory(MathGeoLib)
add_subdirectory(EyeTribe)
add_subdirectory(VVRScene)
add_subdirectory(VVRPhysics)
#########################################################################################

set_property(TARGET VVRScene PROPERTY FOLDER "VVRLibraries")
set_property(TARGET VVRPhysics PROPERTY FOLDER "VVRLibraries")
set_property(TARGET GeoLib PROPERTY FOLDER "VVRLibraries")
set_property(TARGET MathGeoLib PROPERTY FOLDER "VVRLibraries")
set_property(TARGET GazeApiLib PROPERTY FOLDER "VVRLibraries")

### Organize targets into folders #######################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#########################################################################################

### Warning options #####################################################################
option(DISABLE_WARNINGS "Disable Warnings MSVC/GCC" ON)
if(DISABLE_WARNINGS)
 set(MSVC_WARN_LEVEL /W0)
 set(GCC_WARN_LEVEL -w)
else()
 set(MSVC_WARN_LEVEL /W3)
 set(GCC_WARN_LEVEL -Wall -Wno-long-long -pedantic)
endif(DISABLE_WARNINGS) 
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "${MSVC_WARN_LEVEL}" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_WARN_LEVEL}")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_WARN_LEVEL}")
endif()
#########################################################################################