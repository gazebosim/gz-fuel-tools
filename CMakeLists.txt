cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(gz-fuel_tools9 VERSION 9.0.0)

#============================================================================
# Find gz-cmake
#============================================================================
# If you get an error at this line, you need to install gz-cmake
find_package(gz-cmake3 REQUIRED)
set(GZ_CMAKE_VER ${gz-cmake3_VERSION_MAJOR})

#============================================================================
# Configure the project
#============================================================================
gz_configure_project(VERSION_SUFFIX pre2)

#============================================================================
# Set project-specific options
#============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#============================================================================
# Search for project-specific dependencies
#============================================================================

#--------------------------------------
# Find Tinyxml2
gz_find_package(TINYXML2 REQUIRED PRIVATE PRETTY tinyxml2)

#--------------------------------------
# Find libcurl
gz_find_package(GzCURL REQUIRED PRIVATE PRETTY curl)

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
# Find gz-utils
gz_find_package(gz-utils2 REQUIRED)
set(GZ_UTILS_VER ${gz-utils2_VERSION_MAJOR})

#--------------------------------------
# Find gz-common
gz_find_package(gz-common5 REQUIRED PRIVATE COMPONENTS testing)
set(GZ_COMMON_VER ${gz-common5_VERSION_MAJOR})

#--------------------------------------
# Find gz-math
gz_find_package(gz-math7 REQUIRED PRIVATE)
set(GZ_MATH_VER ${gz-math7_VERSION_MAJOR})

#--------------------------------------
# Find gz-msgs
gz_find_package(gz-msgs10 REQUIRED PRIVATE)
set(GZ_MSGS_VER ${gz-msgs10_VERSION_MAJOR})

#--------------------------------------
# Find gz-tools
find_program(HAVE_GZ_TOOLS gz)
set (GZ_TOOLS_VER 2)

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
   "${GZ-COMMON_DOXYGEN_TAGFILE} = ${GZ-COMMON_API_URL}"
  )
