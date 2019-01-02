set(TEST_TYPE "INTEGRATION")

set(tests
  zip.cc
)

include_directories(SYSTEM ${CMAKE_BINARY_DIR}/test/)
link_directories(${PROJECT_BINARY_DIR}/test)

ign_build_tests(TYPE INTEGRATION
                SOURCES ${tests}
                LIB_DEPS ignition-common${IGN_COMMON_MAJOR_VER}::ignition-common${IGN_COMMON_MAJOR_VER}
)
