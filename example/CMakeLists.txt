cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Find the Ignition Fuel Tools library
find_package(ignition-fuel_tools4 QUIET REQUIRED)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-FUEL-TOOLS_CXX_FLAGS}")
include_directories(
  ${IGNITION-COMMON_INCLUDE_DIRS}
  ${IGNITION-FUEL_TOOLS_INCLUDE_DIRS}
)
link_directories(
  ${IGNITION-COMMON_LIBRARY_DIRS}
  ${IGNITION-FUEL_TOOLS_LIBRARY_DIRS}
)

# GFlags
find_library(gflags_LIBRARIES NAMES gflags)
find_path(gflags_INCLUDE_DIRS gflags/gflags.h ENV CPATH)
if (NOT gflags_LIBRARIES OR NOT gflags_INCLUDE_DIRS)
  message (STATUS "Unable to compile 'bench.cc', missing GFLags library")
else()
  include_directories(SYSTEM ${gflags_INCLUDE_DIRS})
endif()

# Generate examples
if (EXISTS "${CMAKE_SOURCE_DIR}/list.cc")
  add_executable(list list.cc)
  target_link_libraries(list
                        ${IGNITION-COMMON_LIBRARIES}
                        ${IGNITION-FUEL_TOOLS_LIBRARIES}
                        ${gflags_LIBRARIES})
endif()

if (EXISTS "${CMAKE_SOURCE_DIR}/download.cc")
  add_executable(download download.cc)
  target_link_libraries(download
                        ${IGNITION-COMMON_LIBRARIES}
                        ${IGNITION-FUEL_TOOLS_LIBRARIES}
                        ${gflags_LIBRARIES})
endif()

if (EXISTS "${CMAKE_SOURCE_DIR}/details.cc")
  add_executable(details details.cc)
  target_link_libraries(details
                        ${IGNITION-COMMON_LIBRARIES}
                        ${IGNITION-FUEL_TOOLS_LIBRARIES}
                        ${gflags_LIBRARIES})
endif()

if (MSVC)

  # Don't pull in the Windows min/max macros
  add_definitions(-DNOMINMAX)

endif()
