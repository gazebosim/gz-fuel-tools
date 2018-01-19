cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-fuel_tools1 VERSION 1.0.0)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake0 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
ign_configure_project(VERSION_SUFFIX pre1)

#============================================================================
# Set project-specific options
#============================================================================

# ignition-fuel-tools currently has no options that are unique to it

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
ign_find_package(ignition-common1 REQUIRED PRIVATE)

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
