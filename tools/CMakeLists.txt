include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/test
)

set (test_sources

)

if (IGNITION-TOOLS_BINARY_DIRS)
  ign_build_tests(${test_sources})
endif ()
