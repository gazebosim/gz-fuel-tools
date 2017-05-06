cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Find the Ignition Fuel Tools library
find_package(ignition-fuel-tools0 QUIET REQUIRED)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-RNDF_CXX_FLAGS}")
include_directories(${IGNITION-FUEL-TOOLS_INCLUDE_DIRS})
link_directories(${IGNITION-FUEL-TOOLS_LIBRARY_DIRS})

# Generate examples

if (MSVC)

  # Don't pull in the Windows min/max macros
  add_definitions(-DNOMINMAX)

endif()
