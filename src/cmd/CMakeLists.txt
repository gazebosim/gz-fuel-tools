include (${project_cmake_dir}/Utils.cmake)

set(cmd_lib_name "IgnitionFuelCMD${PROJECT_MAJOR_VERSION}")

add_library(${cmd_lib_name} IgnitionFuelCMD.cc)
target_link_libraries(${cmd_lib_name} "${PROJECT_NAME}${PROJECT_MAJOR_VERSION}")
ign_install_library(${cmd_lib_name})

if (APPLE)
  set(IGN_FUEL_CMD_LIB_INSTALL_PATH ${cmd_lib_name}.dylib)
elseif (WIN32)
  set(IGN_FUEL_CMD_LIB_INSTALL_PATH ${cmd_lib_name}.dll)
else()
  set(IGN_FUEL_CMD_LIB_INSTALL_PATH lib${cmd_lib_name}.so)
endif()

# Generate a the ruby script.
# Note that the major version of the library is included in the name.
# Ex: cmdtransport0.rb
if (APPLE)
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}.dylib)
else()
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}.so)
endif()

configure_file(
  "cmdfuel.rb.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmdfuel${PROJECT_MAJOR_VERSION}.rb" @ONLY)


# Install the ruby command line library in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmdfuel${PROJECT_MAJOR_VERSION}.rb DESTINATION lib/ruby/ignition)
