set (sources
  ClientConfig.cc
  CollectionIdentifier.cc
  FuelClient.cc
  ign.cc
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
  ign_src_TEST.cc
  Interface_TEST.cc
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

if(NOT USE_EXTERNAL_TINYXML2)
  # If we are using our internal copy of tinyxml2, then add its
  # source file.
  message(STATUS "Adding tinyxml2 source files")
  list(APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2/tinyxml2.cpp)
endif()


if (HAVE_IGN_TOOLS)
  set (gtest_sources ${gtest_sources}
    ign_TEST.cc
  )
endif()

# Create the library target.
ign_create_core_library(SOURCES ${sources} CXX_STANDARD 17)

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

if(USE_EXTERNAL_TINYXML2)

  # If we are using an external copy of tinyxml2, add its imported target
  target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
    PRIVATE
      TINYXML2::TINYXML2)

    # The collada exporter test uses tinyxml2, so we must link it if we're using
    # an external copy. The graphics target considers tinyxml2 to be a private
    # dependency, so it will not automatically get linked to this test.
    # if(TARGET UNIT_ColladaExporter_TEST)
    #   target_link_libraries(UNIT_ColladaExporter_TEST TINYXML2::TINYXML2)
    # endif()
else()
  # Otherwise, add its source directory to our target's PRIVATE include
  # directories. We do not want this to be visible to consumers of
  # ignition-fuel-tools.
  target_include_directories(${PROJECT_LIBRARY_TARGET_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2)
  # if(TARGET UNIT_ColladaExporter_TEST)

  #   # If we are using the internal copy of tinyxml2, then the collada exporter
  #   # test needs to be pointed to the internal tinyxml2 include directory.
  #   target_include_directories(UNIT_ColladaExporter_TEST PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2)
  #   # We also need to add this as a source file to the collada exporter test's
  #   # target, because we do not export the tinyxml2 symbols that get generated
  #   # in the ignition-common library. Therefore, the collada exporter test
  #   # cannot link to those symbols and must generate its own.
  #   target_sources(UNIT_ColladaExporter_TEST PRIVATE tinyxml2/tinyxml2.cpp)

  # endif()

endif()

ign_target_interface_include_directories(${PROJECT_LIBRARY_TARGET_NAME}
  ignition-common${IGN_COMMON_MAJOR_VER}::ignition-common${IGN_COMMON_MAJOR_VER}
  ignition-msgs${IGN_MSGS_MAJOR_VER}::ignition-msgs${IGN_MSGS_MAJOR_VER}
)


# Build the unit tests.
ign_build_tests(TYPE UNIT
                SOURCES ${gtest_sources}
                LIB_DEPS
                  ignition-cmake${IGN_CMAKE_VER}::utilities
                  ignition-common${IGN_COMMON_MAJOR_VER}::ignition-common${IGN_COMMON_MAJOR_VER}
)

# Command line support.
add_subdirectory(cmd)
