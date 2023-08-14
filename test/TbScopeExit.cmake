find_package(Catch2 3 REQUIRED)
add_executable(tb_scope_exit_test ${CMAKE_CURRENT_LIST_DIR}/src/main.cc)
target_link_libraries(tb_scope_exit_test PRIVATE Catch2::Catch2WithMain tb_scope_exit)
