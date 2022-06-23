set (sources
  ClientConfig.cc
  CollectionIdentifier.cc
  FuelClient.cc
  Helpers.cc
  gz.cc
  Interface.cc
  JSONParser.cc
  LocalCache.cc
  Model.cc
  ModelIdentifier.cc
  ModelIter.cc
  RestClient.cc
  Result.cc
  Zip.cc
  WorldIdentifier.cc
  WorldIter.cc
)

set (gtest_sources
  ClientConfig_TEST.cc
  CollectionIdentifier_TEST.cc
  FuelClient_TEST.cc
  gz_src_TEST.cc
  Interface_TEST.cc
  Helpers_TEST.cc
  JSONParser_TEST.cc
  LocalCache_TEST.cc
  ModelIdentifier_TEST.cc
  ModelIter_TEST.cc
  Model_TEST.cc
  RestClient_TEST.cc
  Result_TEST.cc
  WorldIdentifier_TEST.cc
  WorldIter_TEST.cc
  Zip_TEST.cc
)

if (HAVE_GZ_TOOLS)
  set (gtest_sources ${gtest_sources}
    gz_TEST.cc
  )
endif()

# Create the library target.
gz_create_core_library(SOURCES ${sources} CXX_STANDARD 17)

# Link the libraries that we always need.
target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC
    ignition-common${IGN_COMMON_MAJOR_VER}::ignition-common${IGN_COMMON_MAJOR_VER}
  PRIVATE
    ignition-msgs${IGN_MSGS_MAJOR_VER}::ignition-msgs${IGN_MSGS_MAJOR_VER}
    curl::curl
    ${JSONCPP_TARGET}
    TINYXML2::TINYXML2
    ${YAML_TARGET}
    ZIP::ZIP
)

gz_target_interface_include_directories(${PROJECT_LIBRARY_TARGET_NAME}
  ignition-common${IGN_COMMON_MAJOR_VER}::ignition-common${IGN_COMMON_MAJOR_VER}
  ignition-msgs${IGN_MSGS_MAJOR_VER}::ignition-msgs${IGN_MSGS_MAJOR_VER}
)


# Build the unit tests.
gz_build_tests(TYPE UNIT
                SOURCES ${gtest_sources}
                TEST_LIST test_targets
                LIB_DEPS
                  ignition-common${IGN_COMMON_MAJOR_VER}::ignition-common${IGN_COMMON_MAJOR_VER}
                  TINYXML2::TINYXML2
)

foreach(test ${test_targets})

  target_compile_definitions(${test} PRIVATE
    "GZ_PATH=\"${HAVE_GZ_TOOLS}\"")

endforeach()

# Command line support.
add_subdirectory(cmd)
