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
else()
  # Otherwise, add its source directory to our target's PRIVATE include
  # directories. We do not want this to be visible to consumers of
  # ignition-fuel-tools.
  target_include_directories(${PROJECT_LIBRARY_TARGET_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2)
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
