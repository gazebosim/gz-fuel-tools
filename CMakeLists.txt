cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-fuel_tools8 VERSION 8.0.0)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake3 REQUIRED)
set(IGN_CMAKE_VER ${ignition-cmake3_VERSION_MAJOR})

#============================================================================
# Configure the project
#============================================================================
gz_configure_project(
  REPLACE_IGNITION_INCLUDE_PATH gz/fuel_tools
  VERSION_SUFFIX pre1)

#============================================================================
# Set project-specific options
#============================================================================

#============================================================================
# Search for project-specific dependencies
#============================================================================

#--------------------------------------
# Find Tinyxml2
gz_find_package(TINYXML2 REQUIRED PRIVATE PRETTY tinyxml2)

#--------------------------------------
# Find libcurl
gz_find_package(IgnCURL REQUIRED PRIVATE PRETTY curl)

#--------------------------------------
# Find jsoncpp
gz_find_package(JSONCPP REQUIRED PRIVATE)

#--------------------------------------
# Find libyaml
gz_find_package(YAML REQUIRED PRIVATE)

#--------------------------------------
# Find libzip
gz_find_package(ZIP REQUIRED PRIVATE)

#--------------------------------------
# Find ignition-utils
gz_find_package(ignition-utils2 REQUIRED)
set(IGN_UTILS_VER ${ignition-utils2_VERSION_MAJOR})

#--------------------------------------
# Find ignition-common
gz_find_package(ignition-common5 REQUIRED PRIVATE)
set(IGN_COMMON_MAJOR_VER ${ignition-common5_VERSION_MAJOR})

#--------------------------------------
# Find ignition-math
gz_find_package(ignition-math7 REQUIRED PRIVATE)
set(IGN_MSGS_MAJOR_VER ${ignition-math7_VERSION_MAJOR})

#--------------------------------------
# Find ignition-msgs
gz_find_package(ignition-msgs9 REQUIRED PRIVATE)
set(IGN_MSGS_MAJOR_VER ${ignition-msgs9_VERSION_MAJOR})

#--------------------------------------
# Find ignition-tools
find_program(HAVE_GZ_TOOLS gz)

#============================================================================
# Configure the build
#============================================================================
gz_configure_build(QUIT_IF_BUILD_ERRORS)

#============================================================================
# gz command line support
#============================================================================
add_subdirectory(conf)

#============================================================================
# Create package information
#============================================================================
gz_create_packages()

#============================================================================
# Configure documentation
#============================================================================
configure_file(${CMAKE_SOURCE_DIR}/api.md.in ${CMAKE_BINARY_DIR}/api.md)
configure_file(${CMAKE_SOURCE_DIR}/tutorials.md.in ${CMAKE_BINARY_DIR}/tutorials.md)

gz_create_docs(
  API_MAINPAGE_MD "${CMAKE_BINARY_DIR}/api.md"
  TUTORIALS_MAINPAGE_MD "${CMAKE_BINARY_DIR}/tutorials.md"
  TAGFILES
   "${IGNITION-COMMON_DOXYGEN_TAGFILE} = ${IGNITION-COMMON_API_URL}"
  )
