cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(ignition-fuel_tools0 VERSION 0.1.4)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake0 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
ign_configure_project()


#============================================================================
# Set project-specific options
#============================================================================

# ignition-cmake currently has no options that are unique to it


#============================================================================
# Search for project-specific dependencies
#============================================================================

#--------------------------------------
# Find libcurl
ign_find_package(CURL REQUIRED)

#--------------------------------------
# Find jsoncpp
ign_find_package(JSONCPP REQUIRED)

#--------------------------------------
# Find libyaml
ign_find_package(YAML REQUIRED)

#--------------------------------------
# Find libzip
ign_find_package(ZIP REQUIRED)

#--------------------------------------
# Find ignition-common
ign_find_package(ignition-common1 REQUIRED)

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
