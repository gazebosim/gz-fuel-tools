set(cmd_lib_name "IgnitionFuelCMD${PROJECT_VERSION_MAJOR}")

add_library(${cmd_lib_name} IgnitionFuelCMD.cc)
target_link_libraries(${cmd_lib_name} "${PROJECT_NAME}${PROJECT_VERSION_MAJOR}")

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
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}${PROJECT_VERSION_MAJOR}.dylib)
else()
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}${PROJECT_VERSION_MAJOR}.so)
endif()

configure_file(
  "cmdfuel.rb.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmdfuel${PROJECT_VERSION_MAJOR}.rb" @ONLY)


# Install the ruby command line library in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmdfuel${PROJECT_VERSION_MAJOR}.rb DESTINATION lib/ruby/ignition)
