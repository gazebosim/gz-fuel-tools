cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Find the Ignition Fuel Tools library
find_package(ignition-fuel_tools1 QUIET REQUIRED)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-FUEL-TOOLS_CXX_FLAGS}")
include_directories(${IGNITION-FUEL_TOOLS_INCLUDE_DIRS})
link_directories(${IGNITION-FUEL_TOOLS_LIBRARY_DIRS})

# GFlags
find_library(gflags_LIBRARIES NAMES gflags)
find_path(gflags_INCLUDE_DIRS gflags/gflags.h ENV CPATH)
if (NOT gflags_LIBRARIES OR NOT gflags_INCLUDE_DIRS)
  message (STATUS "Unable to compile 'bench.cc', missing GFLags library")
else()
  include_directories(SYSTEM ${gflags_INCLUDE_DIRS})
endif()

# Generate examples
add_executable(modelList modelList.cc)
target_link_libraries(modelList
                      ${IGNITION-FUEL_TOOLS_LIBRARIES}
                      ${gflags_LIBRARIES})

add_executable(modelDownload modelDownload.cc)
target_link_libraries(modelDownload
                      ${IGNITION-FUEL_TOOLS_LIBRARIES}
                      ${gflags_LIBRARIES})

add_executable(modelDetails modelDetails.cc)
target_link_libraries(modelDetails
                      ${IGNITION-FUEL_TOOLS_LIBRARIES}
                      ${gflags_LIBRARIES})

if (MSVC)

  # Don't pull in the Windows min/max macros
  add_definitions(-DNOMINMAX)

endif()
