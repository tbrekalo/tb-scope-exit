find_package(Catch2 3 QUIET)
if (NOT Catch2_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0)

  FetchContent_MakeAvailable(Catch2)
endif ()

add_executable(tb_scope_exit_test ${CMAKE_CURRENT_LIST_DIR}/src/main.cc)
target_link_libraries(tb_scope_exit_test PRIVATE Catch2::Catch2WithMain tb_scope_exit)
