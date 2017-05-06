include (${project_cmake_dir}/Utils.cmake)
include (CheckCXXSourceCompiles)

include (${project_cmake_dir}/FindOS.cmake)

########################################
# Include man pages stuff
include (${project_cmake_dir}/Ronn2Man.cmake)
add_manpage_target()

########################################
# Find ignition math
find_package(ignition-math3 QUIET)
if (NOT ignition-math3_FOUND)
  message(STATUS "Looking for ignition-math3-config.cmake - not found")
  BUILD_ERROR ("Missing: Ignition math3 library (libignition-math3-dev).")
else()
  message(STATUS "Looking for ignition-math3-config.cmake - found")
  include_directories(${IGNITION-MATH_INCLUDE_DIRS})
  link_directories(${IGNITION-MATH_LIBRARY_DIRS})
endif()

#################################################
# Macro to check for visibility capability in compiler
# Original idea from: https://gitorious.org/ferric-cmake-stuff/
macro (check_gcc_visibility)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-fvisibility=hidden GCC_SUPPORTS_VISIBILITY)
endmacro()
