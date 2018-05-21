set (sources
  ClientConfig.cc
  FuelClient.cc
  Helpers.cc
  ign.cc
  JSONParser.cc
  LocalCache.cc
  Model.cc
  ModelIdentifier.cc
  ModelIter.cc
  REST.cc
  RestClient.cc
  Result.cc
  Zip.cc
)

set (gtest_sources
  ClientConfig_TEST.cc
  FuelClient_TEST.cc
  ign_src_TEST.cc
  JSONParser_TEST.cc
  LocalCache_TEST.cc
  ModelIdentifier_TEST.cc
  ModelIter_TEST.cc
  Model_TEST.cc
  Result_TEST.cc
  Zip_TEST.cc
)

if (HAVE_IGN_TOOLS)
  set (gtest_sources ${gtest_sources}
    ign_TEST.cc
  )
endif()

# Create the library target.
ign_add_library(${PROJECT_LIBRARY_TARGET_NAME} ${sources})

# Turn on C++11 support for the library.
ign_set_project_public_cxx_standard(11)

# Link the libraries that we always need.
target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC
    ignition-common1::ignition-common1
  PRIVATE
    curl::curl
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
