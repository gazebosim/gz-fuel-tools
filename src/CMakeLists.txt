include (${project_cmake_dir}/Utils.cmake)

set (sources
  ClientConfig.cc
  FuelClient.cc
  LocalCache.cc
  Model.cc
  ModelIdentifier.cc
  ModelIter.cc
  REST.cc
  Result.cc
)

set (gtest_sources
  ClientConfig_TEST.cc
#  FuelClient_TEST.cc
  Model_TEST.cc
  ModelIdentifier_TEST.cc
  ModelIter_TEST.cc
  Result_TEST.cc
)

include_directories(${CMAKE_BINARY_DIR}/ ${CMAKE_BINARY_DIR}/test/)

ign_build_tests(${gtest_sources})

set (libname ${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION})

if (UNIX)
  ign_add_library(${libname} ${sources})
elseif(WIN32)
  add_library(${libname} STATIC ${sources})
endif()

target_link_libraries(${libname}
  ${CURL_LIBRARIES}
  ${IGNITION-COMMON_LIBRARIES}
)

if(WIN32)
  target_link_libraries(${libname}
    ws2_32 Iphlpapi
  )
else()
  target_link_libraries(${libname}
  )
endif()

add_executable(temp_fuel_client
  temp_fuel_client.cc
)
target_link_libraries(temp_fuel_client ${libname})


ign_install_library(${libname})

add_subdirectory(cmd)
