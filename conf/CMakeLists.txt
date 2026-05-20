set(gz_library_path "${CMAKE_BINARY_DIR}/test/lib/$<CONFIG>/ruby/gz/cmdfuel${PROJECT_VERSION_MAJOR}")

# Generate a configuration file for internal testing.
# Note that the major version of the library is included in the name.
# Ex: fuel0.yaml
configure_file(
  "fuel.yaml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/fuel${PROJECT_VERSION_MAJOR}.yaml.configured" @ONLY)

file(GENERATE
  OUTPUT "${CMAKE_BINARY_DIR}/test/conf/$<CONFIG>/fuel${PROJECT_VERSION_MAJOR}.yaml"
  INPUT "${CMAKE_CURRENT_BINARY_DIR}/fuel${PROJECT_VERSION_MAJOR}.yaml.configured")

set(gz_library_path "${CMAKE_INSTALL_PREFIX}/lib/ruby/gz/cmdfuel${PROJECT_VERSION_MAJOR}")

# Generate a configuration file.
# Note that the major version of the library is included in the name.
# Ex: fuel0.yaml
configure_file(
  "fuel.yaml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/fuel${PROJECT_VERSION_MAJOR}.yaml" @ONLY)

# Install the yaml configuration files in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fuel${PROJECT_VERSION_MAJOR}.yaml DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/gz/)
