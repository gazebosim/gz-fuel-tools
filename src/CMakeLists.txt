include (${project_cmake_dir}/Utils.cmake)

set (sources
  ClientConfig.cc
  FuelClient.cc
  ModelIdentifier.cc
  ModelIter.cc
  Result.cc
)

# set (gtest_sources
#   GirderClient_TEST.cc
# )

include_directories(${CMAKE_BINARY_DIR}/ ${CMAKE_BINARY_DIR}/test/)

# ign_build_tests(${gtest_sources})

if (UNIX)
  ign_add_library(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION} ${sources})
elseif(WIN32)
  add_library(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION} STATIC ${sources})
endif()

target_link_libraries(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}
  ${CURL_LIBRARIES}
)

if(WIN32)
  target_link_libraries(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}
    ws2_32 Iphlpapi
  )
else()
  target_link_libraries(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}
  )
endif()

ign_install_library(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION})

add_subdirectory(cmd)
