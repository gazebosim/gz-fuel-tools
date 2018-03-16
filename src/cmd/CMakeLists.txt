# Generate a the ruby script.
# Note that the major version of the library is included in the name.
# Ex: cmdfuel0.rb
if (APPLE)
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}.dylib)
else()
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}.so)
endif()

configure_file(
  "cmdfuel.rb.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmdfuel${PROJECT_VERSION_MAJOR}.rb" @ONLY)


# Install the ruby command line library in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmdfuel${PROJECT_VERSION_MAJOR}.rb DESTINATION lib/ruby/ignition)
