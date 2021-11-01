#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "visitors/bytecode.h"
#include "vm/vm.h"
#include <stdio.h>
#include "debug/timer.h"
#include "debug/logger.h"

const char* source = "123 + 456 * -1";

int main() {    
    TIMER_START();
    Lexer* lexer = lexer_init();
    Token* tokens = lexer_parse(lexer, source);

    Token token;
    size_t i = 0;
    while((token = tokens[i++]).type != END_OF_FILE) {
        token_print(&token);
    }

    Parser* parser = parser_init(tokens);
    AST* root = parser_parse(parser);

    ast_dump(root);
    printf("\n");

    BytecodeVisitor bcv;
    bcv.bytecode_size = 0;
    bcv.constants_size = 0;

    bytecode_parse_ast(&bcv, root);

    VM vm;
    TIMER_START_NAME(print_copy_constants);
    for(int i = 0; i < bcv.constants_size; i++) {
        vm.constants[i] = bcv.constants[i];
        printf("    ");
        value_dump(&bcv.constants[i]);
        printf("\n");
    }
    TIMER_END_NAME(print_copy_constants);

    FLOG("BYTECODE SIZE: %ld\n", bcv.bytecode_size);

    vm.bytecode = bcv.bytecode;
    
    vm_execute(&vm);

    LOG("FINISHED STACK DUMP: \n");
    stack_dump(&vm.stack);
    
    printf("\nreturn value: %d\n", stack_pop(&vm.stack).as.integer);
    TIMER_END();

    printf("Timings: \n");
    TIMER_DUMP();

    return 0;
}

