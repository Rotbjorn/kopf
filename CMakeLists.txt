cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(kopf C)

include_directories(include)

set(KOPF_SOURCES
     src/ast.c
     src/io.c
     src/lexer.c
     src/main.c
     src/parser.c
     src/token.c

     src/vm/op.c
     src/vm/stack.c
     src/vm/table.c
     src/vm/value.c
     src/vm/vm.c

     src/debug/timer.c

     src/visitors/bytecode.c
)

add_executable(${PROJECT_NAME} ${KOPF_SOURCES})
target_link_libraries( ${PROJECT_NAME} skrivarn )
