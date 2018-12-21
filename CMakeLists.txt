cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-fuel_tools3 VERSION 3.0.0)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake2 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
ign_configure_project()

#============================================================================
# Set project-specific options
#============================================================================

#--------------------------------------
# Option: Should we use our internal copy of tinyxml2?
if(UNIX OR APPLE)
  option(USE_EXTERNAL_TINYXML2 "Use a system-installed version of tinyxml2" ON)
elseif(WIN32)
  # This is always false for Windows, so we can avoid trying to search for a
  # system installation of tinyxml2.
  set(USE_EXTERNAL_TINYXML2 false)
endif()

#============================================================================
# Search for project-specific dependencies
#============================================================================

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
# Find ignition-tools
ign_find_package(ignition-tools QUIET)
if (ignition-tools_FOUND)
  set (HAVE_IGN_TOOLS TRUE)
endif()

#--------------------------------------
# Find Tinyxml2
if(USE_EXTERNAL_TINYXML2)
  ign_find_package(TINYXML2 PRETTY tinyxml2
    REQUIRED_BY graphics
    PRIVATE_FOR graphics)
else()
  message(STATUS "Skipping search for tinyxml2 and using an internal version\n")
endif()

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
