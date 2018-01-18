# Collect source files into the "sources" variable and unit test files into the
# "gtest_sources" variable.
ign_get_libsources_and_unittests(sources gtest_sources)

# Create the library target.
ign_add_library(${PROJECT_LIBRARY_TARGET_NAME} ${sources})

# Turn on C++11 support for the library.
ign_set_project_public_cxx_standard(11)

# Link the libraries that we always need.
target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC
  PRIVATE
    curl::curl
    ignition-common1::ignition-common1
    ${JSONCPP_TARGET}
    YAML::YAML
    ZIP::ZIP
)

# Create installation instructions for the library target. This must be called
# in the same scope that the target is created.
ign_install_library()

# Build the unit tests.
ign_build_tests(TYPE UNIT
                SOURCES ${gtest_sources}
                LIB_DEPS ignition-common1::ignition-common1
)

# Command line support.
add_subdirectory(cmd)
