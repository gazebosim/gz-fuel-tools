set(TEST_TYPE "INTEGRATION")

set(tests

)

include_directories(SYSTEM ${CMAKE_BINARY_DIR}/test/)
link_directories(${PROJECT_BINARY_DIR}/test)

ign_build_tests(${tests})
