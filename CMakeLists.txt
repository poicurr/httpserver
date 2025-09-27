cmake_minimum_required(VERSION 3.16)

project(HttpServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(STRINGUTILS_ENABLE_TESTS OFF CACHE INTERNAL "")
FetchContent_Declare(
  StringUtils
  GIT_REPOSITORY https://github.com/poicurr/StringUtils.git
  GIT_TAG 734dbff769bab0d935649f4f8a53e69439d28826
)
FetchContent_MakeAvailable(StringUtils)

add_library(HttpServer INTERFACE)

target_include_directories(HttpServer INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/HttpServer>
  $<INSTALL_INTERFACE:include>
  $<INSTALL_INTERFACE:include/HttpServer>
)

target_link_libraries(HttpServer INTERFACE StringUtils)

if (WIN32)
  target_link_libraries(HttpServer INTERFACE wsock32 ws2_32)
endif ()

option(HTTP_SERVER_BUILD_EXAMPLES "Build example applications" OFF)
if (HTTP_SERVER_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()

