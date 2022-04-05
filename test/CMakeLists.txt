configure_file(test_config.h.in ${PROJECT_BINARY_DIR}/test_config.h)

# Build gtest
add_library(gtest STATIC gtest/src/gtest-all.cc)
add_library(gtest_main STATIC gtest/src/gtest_main.cc)
target_include_directories(gtest
  SYSTEM PUBLIC
  ${PROJECT_SOURCE_DIR}/test/gtest/include
  PRIVATE
  ${PROJECT_SOURCE_DIR}/test/gtest
)

target_link_libraries(gtest_main gtest)
set_property(TARGET gtest_main PROPERTY CXX_STANDARD ${c++standard})
set_property(TARGET gtest PROPERTY CXX_STANDARD ${c++standard})
set(GTEST_LIBRARY "${PROJECT_BINARY_DIR}/test/libgtest.a")
set(GTEST_MAIN_LIBRARY "${PROJECT_BINARY_DIR}/test/libgtest_main.a")

execute_process(COMMAND cmake -E remove_directory ${CMAKE_BINARY_DIR}/test_results)
execute_process(COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/test_results)

add_subdirectory(integration)
add_subdirectory(performance)
add_subdirectory(regression)
