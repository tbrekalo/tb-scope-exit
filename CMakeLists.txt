cmake_minimum_required(VERSION 3.22)
project(tb_scope_exit VERSION 0.1)

option(TB_SCOPE_EXIT_BUILD_TEST "Build catch2 tests" ${PROJECT_IS_TOP_LEVEL})

add_library(tb_scope_exit INTERFACE)
add_library(tb::scope_exit ALIAS tb_scope_exit)
target_include_directories(
  tb_scope_exit INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                          $<INSTALL_INTERFACE:include>)
target_compile_features(tb_scope_exit INTERFACE cxx_std_23)

if(TB_SCOPE_EXIT_BUILD_TEST)
  include(${CMAKE_CURRENT_LIST_DIR}/test/TbScopeExit.cmake)
endif()
