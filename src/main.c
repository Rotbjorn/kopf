#include "debug/timer.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "visitors/bytecode.h"
#include "vm/vm.h"
#include <stdio.h>
#include <rotbjorn/skrivarn.h>

const char* source = "123.0 + 321.0 * -0.5";

int main() {    
    TIMER_START();
    Lexer* lexer = lexer_init();

    TIMER_START_NAME(Lexer);
    Token* tokens = lexer_parse(lexer, source);
    TIMER_END_NAME(Lexer);


    Token token;
    size_t i = 0;
    while((token = tokens[i++]).type != END_OF_FILE) {
        token_print(&token);
    }

    TIMER_START_NAME(Parser);
    Parser* parser = parser_init(tokens);
    AST* root = parser_parse(parser);
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
    for(int i = 0; i < bcv.constants_size; i++) {
        vm.constants[i] = bcv.constants[i];
        printf("    ");
        value_dump(&bcv.constants[i]);
        printf("\n");
    }
    TIMER_END_NAME(print_copy_constants);

    skrivarn_infof("Bytecode size: %ld", bcv.bytecode_size);

    vm.bytecode = bcv.bytecode;
    
    TIMER_START_NAME(Runtime);
    vm_execute(&vm);
    TIMER_END_NAME(Runtime);

    skrivarn_info("FINISHED STACK DUMP: ");
    stack_dump(&vm.stack);
    
    printf("\nreturn value: %d\n", stack_pop(&vm.stack).as.integer);
    TIMER_END();

    printf("Timings: \n");
    TIMER_DUMP();

    return 0;
}

