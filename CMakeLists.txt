cmake_minimum_required(VERSION 3.18)
project(tb_scope_exit VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
option(TB_SCOPE_EXIT_BUILD_TEST "Build catch2 tests" OFF)
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(TB_SCOPE_EXIT_BUILD_TEST ON)
endif()

add_library(tb_scope_exit INTERFACE)
add_library(tb::scope_exit ALIAS tb_scope_exit)
target_include_directories(
  tb_scope_exit INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                          $<INSTALL_INTERFACE:include>)

if(TB_SCOPE_EXIT_BUILD_TEST)
  include(${CMAKE_CURRENT_LIST_DIR}/test/TbScopeExit.cmake)
endif()
