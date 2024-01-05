cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-fuel_tools4 VERSION 4.9.1)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake2 2.13 REQUIRED)
set(IGN_CMAKE_VER ${ignition-cmake2_VERSION_MAJOR})

#============================================================================
# Configure the project
#============================================================================
ign_configure_project(
  REPLACE_IGNITION_INCLUDE_PATH gz/fuel_tools
)

#============================================================================
# Set project-specific options
#============================================================================

#============================================================================
# Search for project-specific dependencies
#============================================================================

#--------------------------------------
# Find Tinyxml2
ign_find_package(TINYXML2 REQUIRED PRIVATE PRETTY tinyxml2)

#--------------------------------------
# Find libcurl
ign_find_package(IgnCURL REQUIRED PRIVATE)

#--------------------------------------
# Find jsoncpp
ign_find_package(JSONCPP REQUIRED PRIVATE)

#--------------------------------------
# Find libyaml
ign_find_package(YAML REQUIRED PRIVATE)

#--------------------------------------
# Find libzip
ign_find_package(ZIP REQUIRED PRIVATE)

#--------------------------------------
# Find ignition-common
ign_find_package(ignition-common3 REQUIRED PRIVATE)
set(IGN_COMMON_MAJOR_VER ${ignition-common3_VERSION_MAJOR})

#--------------------------------------
# Find ignition-math
ign_find_package(ignition-math6 REQUIRED PRIVATE)
set(IGN_MSGS_MAJOR_VER ${ignition-math6_VERSION_MAJOR})

#--------------------------------------
# Find ignition-msgs
ign_find_package(ignition-msgs5 REQUIRED PRIVATE)
set(IGN_MSGS_MAJOR_VER ${ignition-msgs5_VERSION_MAJOR})

#--------------------------------------
# Find ignition-tools
ign_find_package(ignition-tools QUIET)
if (ignition-tools_FOUND)
  set (HAVE_IGN_TOOLS TRUE)
endif()
set (IGN_TOOLS_VER 1)

#============================================================================
# Configure the build
#============================================================================
ign_configure_build(QUIT_IF_BUILD_ERRORS)

#============================================================================
# ign command line support
#============================================================================
add_subdirectory(conf)

#============================================================================
# Create package information
#============================================================================
ign_create_packages()

#============================================================================
# Configure documentation
#============================================================================
configure_file(${CMAKE_SOURCE_DIR}/api.md.in ${CMAKE_BINARY_DIR}/api.md)
configure_file(${CMAKE_SOURCE_DIR}/tutorials.md.in ${CMAKE_BINARY_DIR}/tutorials.md)

ign_create_docs(
  API_MAINPAGE_MD "${CMAKE_BINARY_DIR}/api.md"
  TUTORIALS_MAINPAGE_MD "${CMAKE_BINARY_DIR}/tutorials.md"
  TAGFILES
   "${IGNITION-COMMON_DOXYGEN_TAGFILE} = ${IGNITION-COMMON_API_URL}"
  )
