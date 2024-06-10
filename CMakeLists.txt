cmake_minimum_required(VERSION 3.28)
project(neolang VERSION 24.1.0 DESCRIPTION "Programming Language Learning" LANGUAGES C)

add_executable(${PROJECT_NAME} "src/entry.c" "src/tokenize.c" "src/types.h")

set_target_properties(${PROJECT_NAME} PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED TRUE
  LINKER_LANGUAGE C
  VERSION ${PROJECT_VERSION}
)

target_include_directories(${PROJECT_NAME} PRIVATE "src/")

SET(BASE_COMPILE_FLAGS -Wall -Wextra -Wpedantic -lphread -march=native -flto=auto) 
IF(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${PROJECT_NAME} PRIVATE -g ${BASE_COMPILE_FLAGS})
ELSE()
  target_compile_options(${PROJECT_NAME} PRIVATE -O3 ${BASE_COMPILE_FLAGS})
ENDIF()


