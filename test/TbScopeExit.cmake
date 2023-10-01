find_package(doctest QUIET)

if(NOT doctest_GOUNF)
  include(FetchContent)
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG v2.4.11)
  FetchContent_MakeAvailable(doctest)
endif()

add_executable(tb_scope_exit_test ${CMAKE_CURRENT_LIST_DIR}/src/main.cc)
target_link_libraries(tb_scope_exit_test PRIVATE doctest::doctest)
