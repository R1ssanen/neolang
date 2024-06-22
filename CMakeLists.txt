cmake_minimum_required(VERSION 3.28)
project(
  neolang
  VERSION 24.1.0
  DESCRIPTION "Programming Language Learning"
  LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
add_executable(
  ${PROJECT_NAME}
  "src/entry.c"
  "src/lexer/tokenize.c"
  "src/lexer/token_types.c"
  "src/lexer/token_subtypes.c"
  "src/lexer/tokenizer.c"
  "src/parser/parse.c"
  "src/parser/parser.c"
  "src/gen/generate.c"
  "src/gen/generator.c"
  "src/arena.c"
  "src/debug/ast_output.c"
  "src/debug/jWrite.c")

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES C_STANDARD 11
             C_STANDARD_REQUIRED TRUE
             LINKER_LANGUAGE C
             VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PRIVATE "src/")

set(BASE_COMPILE_FLAGS -Wall -Wextra -Wpedantic -lphread -march=native
                       -flto=auto)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${PROJECT_NAME} PRIVATE -g ${BASE_COMPILE_FLAGS})
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -O3 ${BASE_COMPILE_FLAGS})
endif()
