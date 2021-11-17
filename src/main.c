#include "debug/timer.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "visitors/bytecode.h"
#include "vm/vm.h"
#include <stdio.h>
#include <rotbjorn/skrivarn.h>
#include "vm/table.h"
#include "vm/op.h"
#include "io.h"

const char *source = 
"decimal FORTNITE = 12.0;"
"decimal NIGHTER = 15.0;"
"FORTNITE + NIGHTER";


int main() {
    TIMER_START();
    Lexer *lexer = lexer_init();

    const char* file = read_file("examples/main.kopf");

    TIMER_START_NAME(Lexer);
    Token *tokens = lexer_parse(lexer, file);
    TIMER_END_NAME(Lexer);

    Token token;
    size_t i = 0;
    while ((token = tokens[i++]).type != END_OF_FILE) {
        token_print(&token);
    }

    TIMER_START_NAME(Parser);
    Parser *parser = parser_init(tokens);
    AST *root = parser_generate_ast(parser);
    TIMER_END_NAME(Parser);

    ast_dump(root);
    printf("\n");

    BytecodeVisitor bcv;
    bcv.bytecode_size = 0;
    bcv.constants_size = 0;


    TIMER_START_NAME(BytecodeVisitor);
    bytecode_parse_ast(&bcv, root);
    TIMER_END_NAME(BytecodeVisitor);


    VM vm;
    TIMER_START_NAME(print_copy_constants);
    for (int i = 0; i < bcv.constants_size; i++) {
        vm.constants[i] = bcv.constants[i];
        printf("    ");
        value_dump(&bcv.constants[i]);
        printf("\n");
    }
    TIMER_END_NAME(print_copy_constants);

    skrivarn_infof("Bytecode size: %ld", bcv.bytecode_size);

    for (int i = 0; i < bcv.bytecode_size; i++) {
        printf("%02x ", bcv.bytecode[i]);
    }
    printf("\n");

    vm.bytecode = bcv.bytecode;
    vm.globals = malloc(sizeof(Value) * bcv.global_variables_count);

    TIMER_START_NAME(Runtime);
    vm_execute(&vm);
    TIMER_END_NAME(Runtime);

    skrivarn_info("FINISHED STACK DUMP: ");
    stack_dump(&vm.stack);

    TIMER_END();

    for (int i = 0; i < bcv.global_variables_count; i++) {
        printf("i=%d : ", i);
        value_dump(&vm.globals[i]);
        printf("\n");
    }

    printf("Timings: \n");
    TIMER_DUMP();

    return 0;
}
