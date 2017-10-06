include (${project_cmake_dir}/Utils.cmake)
include (CheckCXXSourceCompiles)

include (${project_cmake_dir}/FindOS.cmake)

########################################
# Include man pages stuff
include (${project_cmake_dir}/Ronn2Man.cmake)
add_manpage_target()

########################################
# Find PkgConfig
find_package(PkgConfig REQUIRED)

########################################
# Find libcurl
find_package(CURL)
if (CURL_FOUND)
  # FindCURL.cmake distributed with CMake exports
  # the CURL_INCLUDE_DIRS variable, while the pkg_check_modules
  # function exports the CURL_INCLUDEDIR variable.
  # TODO: once the configure.bat VS2013 based script has been removed,
  #       remove the call pkg_check_modules(CURL libcurl) and all the uses of
  #       CURL_LIBDIR and CURL_INCLUDEDIR and use directly the variables
  #       CURL_INCLUDE_DIRS and CURL_LIBRARIES provided by FindCURL.cmake
  set(CURL_INCLUDEDIR ${CURL_INCLUDE_DIRS})
endif ()
if (NOT CURL_FOUND)
  pkg_check_modules(CURL libcurl)
endif ()
if (NOT CURL_FOUND)
  BUILD_ERROR ("Missing: libcurl. Required for connection to model database.")
endif()

########################################
# Find jsoncpp
pkg_check_modules(jsoncpp jsoncpp)
if (NOT jsoncpp_FOUND)
  BUILD_ERROR ("Missing: libjsoncpp-dev. Required for parsing json data.")
endif()

########################################
# Find libzip
pkg_check_modules(libzip libzip)

if (NOT libzip_FOUND)
  BUILD_ERROR ("Missing: libzip-dev. Required for parsing compressing files.")
endif()

################################################################################
# Ignition common
find_package(ignition-common0 QUIET)
if (NOT ignition-common0_FOUND)
  BUILD_ERROR ("Missing: Ignition Common (libignition-common0-dev)")
else()
  message (STATUS "Found Ignition Common")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-COMMON_CXX_FLAGS}")
  include_directories(${IGNITION-COMMON_INCLUDE_DIRS})
  link_directories(${IGNITION-COMMON_LIBRARY_DIRS})
endif()

#################################################
# Macro to check for visibility capability in compiler
# Original idea from: https://gitorious.org/ferric-cmake-stuff/
macro (check_gcc_visibility)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-fvisibility=hidden GCC_SUPPORTS_VISIBILITY)
endmacro()
