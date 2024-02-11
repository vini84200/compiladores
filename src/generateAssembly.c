#include "generateAssembly.h"
#include "lib.h"
#include "logs.h"

// Generate assembly code in AT&T syntax

// TODO: Consider using .comm when the variable is not initialized
// TODO: Usar mapeamento de registradores para variáveis locais e escopos para manter esse mapa
// Usar uma lista linkada global com os escopos
// Quando entrar em um escopo, adicionar ele na lista, como copia do anterior
// Quando sair de um escopo, remover ele da lista, e dar merge no anterior, fazendo o merge do mapa de registradores,
// atualizando quem aponta para quem, e liberando os registradores que não são mais usados
// TODO: Permitir a escrita para parametros


#define GEN_ASM(...) fprintf(output_file, ##__VA_ARGS__)
#define COMMENT(text, ...) \
    if (debug) { GEN_ASM("/* " text "*/\n", ##__VA_ARGS__); }

HashEntry *format_string_int;
HashEntry *format_string_char;
HashEntry *format_string_float;

HashEntry *scan_error_string;
HashEntry *scan_format_string_int;

void setFunctionParametersOffsets(HashEntry *fun, FILE *output_file);
/**
 * Generates the assembly code for the global variables
 * @param tac The TAC list
 * @param ast The AST
 * @param output_file The output file
 */
void generateVariableDeclarations(TacList *tac, AST *ast, FILE *output_file) {

    GEN_ASM(".data\n");
    // Generate the assembly code for the global variables
    AST *global_variables = ast->children[0];
    ASTListIterator *declarationsIterator = createASTListIterator(global_variables);
    while (!ASTIteratorDone(declarationsIterator)) {
        AST *declaration = getNextAST(declarationsIterator);
        if (declaration->type == AST_VAR_DECLARATION) {
            // (symb=nome, 0=tipo, 1=valor)
            HashEntry *variable_symbol = declaration->symbol;
            AST *variable_type = declaration->children[0];
            AST *variable_value = declaration->children[1];
            char *var_size;
            if (variable_type->type == AST_TYPE_INT) {
                var_size = ".int";
            } else if (variable_type->type == AST_TYPE_FLOAT) {
                var_size = ".float";
            } else if (variable_type->type == AST_TYPE_CHAR) {
                var_size = ".byte";
            } else {
                ERROR("Invalid variable type");
                continue;
            }
            // TODO: Consider using .comm when the variable is not initialized
            GEN_ASM("%s: %s %s\n", variable_symbol->value, var_size, variable_value->symbol->value);
        }

        if (declaration->type == AST_ARRAY_DECLARATION) {
            // (symb=nome, 0=tipo, 1=size, 2=valores)
            HashEntry *variable_symbol = declaration->symbol;
            AST *variable_type = declaration->children[0];
            AST *variable_size = declaration->children[1];
            AST *variable_values = declaration->children[2];
            char *var_size;
            if (variable_type->type == AST_TYPE_INT) {
                var_size = ".int";
            } else if (variable_type->type == AST_TYPE_FLOAT) {
                var_size = ".float";
            } else if (variable_type->type == AST_TYPE_CHAR) {
                var_size = ".byte";
            } else {
                ERROR("Invalid variable type");
                continue;
            }
            GEN_ASM("%s: \n %s", variable_symbol->value, var_size);
            ASTListIterator *valuesIterator = createASTListIterator(variable_values);
            int count = 0;
            long int size = strtol(variable_size->symbol->value, NULL, 10);
            while (!ASTIteratorDone(valuesIterator)) {
                AST *value = getNextAST(valuesIterator);
                if (count > 0) {
                    GEN_ASM(",");
                }
                GEN_ASM(" %s", value->symbol->value);
                count++;
            }
            destroyASTListIterator(valuesIterator);
            for (int i = count; i < size; i++) {
                if (i > 0) {
                    GEN_ASM(",");
                }
                GEN_ASM(" 0");
            }
            GEN_ASM("\n");
        }
    }
    // Generate the assembly code for the temporary variables
    // TODO: This causes a bug on reentrant functions, where the same temporary variable is used in different functions
    // This will only happen in recursive functions, where the function call is in a expression

    SymbolIterator *symbolIterator = createSymbolIterator(getSymbolTable(), SYMBOL_TEMP_ID);
    while (!SymbolIteratorDone(symbolIterator)) {
        HashEntry *symbol = getNextSymbol(symbolIterator);
        GEN_ASM(".lcomm %s, 4\n", symbol->value);
    }

    destroySymbolIterator(symbolIterator);

    // Generate the assembly code for the format strings
    format_string_int = makeTemp();
    GEN_ASM("%s: .string \"%%d\\n\"\n", format_string_int->value);
    format_string_char = makeTemp();
    GEN_ASM("%s: .string \"%%c\\n\"\n", format_string_char->value);
    format_string_float = makeTemp();
    GEN_ASM("%s: .string \"%%f\\n\"\n", format_string_float->value);
    scan_error_string = makeTemp();
    GEN_ASM("%s: .string \"Error reading input\\n\"\n", scan_error_string->value);
    scan_format_string_int = makeTemp();
    GEN_ASM("%s: .string \"%%d\"\n", scan_format_string_int->value);

    // Generate the assembly code for the string literals
    symbolIterator = createSymbolIterator(getSymbolTable(), SYMBOL_LIT_STRING);
    while (!SymbolIteratorDone(symbolIterator)) {
        HashEntry *symbol = getNextSymbol(symbolIterator);

        HashEntry *strTemp = makeTemp();
        GEN_ASM("%s: .string %s\n", strTemp->value, symbol->value);
        symbol->value = strTemp->value;
    }
    destroySymbolIterator(symbolIterator);
}

void srcIntoRegister(HashEntry *src, FILE *output_file, char *register_name, HashEntry *function_scope) {
    if (src->type == SYMBOL_LIT_INT) {
        GEN_ASM("mov $%s, %%%s\n", src->value, register_name);
    } else if (src->type == SYMBOL_LIT_FLOAT) {
        GEN_ASM("mov $%s, %%%s\n", src->value, register_name);
    } else if (src->type == SYMBOL_LIT_CHAR) {
        GEN_ASM("mov $%s, %%%s\n", src->value, register_name);
    } else if (src->type == SYMBOL_LIT_STRING) {
        GEN_ASM("mov $%s, %%%s\n", src->value, register_name);
    } else if (src->type == SYMBOL_IDENTIFIER) {
        if (src->identifier->bound->bound_type == BOUND_TYPE_GLOBAL) {
            GEN_ASM("mov %s, %%%s\n", src->value, register_name);
        } else {
            if (function_scope == NULL) {
                ERROR("Function scope is NULL and the identifier %s is not global", src->value);
                return;
            }
            IdentifierIterator *identifierIterator = newIdentifierIterator(src->identifier);
            Identifier *identifier = NULL;
            while (!identifierIteratorDone(identifierIterator)) {
                Identifier *ident = getNextIdentifier(identifierIterator);
                if (ident->bound->symbol == function_scope) {
                    identifier = ident;
                    break;
                }
            }
            if (identifier == NULL) {
                ERROR("Could not find identifier %s in function scope %s", src->value, function_scope->value);
                return;
            }
            destroyIdentifierIterator(identifierIterator);
            // This is a local variable, so we need to get the offset from the frame pointer
            GEN_ASM("mov %s_at_%s(%%ebp), %%%s\n", src->value, function_scope->value, register_name);
        }
    } else if (src->type == SYMBOL_TEMP_ID) {
        GEN_ASM("mov %s(,1), %%%s\n", src->value, register_name);
    } else if (src->type == SYMBOL_LABEL) {
        ERROR("Invalid source type LABEL");
    } else {
        ERROR("Invalid source type %d", src->type);
    }
}

void generateFunctions(TacList *tac, FILE *output_file) {
    GEN_ASM(".text\n");
    GEN_ASM(".globl main\n");
    TacIterator tacs = createTacForwardIterator(tac);
    HashEntry *current_function = NULL;
    while (!TacFIterDone(&tacs)) {
        Tac *t = TacFIterNext(&tacs);
        switch (t->op) {
            case TAC_SYMBOL:
                break;
            case TAC_MOVE:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_LABEL:
                GEN_ASM("%s:\n", t->src[0]->value);
                break;
            case TAC_JUMP:
                GEN_ASM("jmp %s\n", t->src[0]->value);
                break;
            case TAC_ADD:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("add %%ebx, %%eax\n");
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_SUB:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("sub %%ebx, %%eax\n");
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_MUL:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("imul %%ebx, %%eax\n");
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_DIV:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("push %%edx\n");
                GEN_ASM("mov $0, %%edx\n");
                GEN_ASM("idiv %%ebx, %%eax\n");
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                GEN_ASM("pop %%edx\n");
                break;
            case TAC_LEQ:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("cmp %%ebx, %%eax\n");
                GEN_ASM("setle %%al\n");
                GEN_ASM("movzx %%al, %%eax\n");// Move the lower byte to the whole eax register
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_LT:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("cmp %%ebx, %%eax\n");
                GEN_ASM("setl %%al\n");        // Set the lower byte of the eax register to 1 if the comparison is true
                GEN_ASM("movzx %%al, %%eax\n");// Move the lower byte to the whole eax register
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_EQ:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("cmp %%ebx, %%eax\n");
                GEN_ASM("sete %%al\n");        // Set the lower byte of the eax register to 1 if the comparison is true
                GEN_ASM("movzx %%al, %%eax\n");// Move the lower byte to the whole eax register
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_NEQ:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("cmp %%ebx, %%eax\n");
                GEN_ASM("setne %%al\n");       // Set the lower byte of the eax register to 1 if the comparison is true
                GEN_ASM("movzx %%al, %%eax\n");// Move the lower byte to the whole eax register
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_GEQ:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("cmp %%ebx, %%eax\n");
                GEN_ASM("setge %%al\n");       // Set the lower byte of the eax register to 1 if the comparison is true
                GEN_ASM("movzx %%al, %%eax\n");// Move the lower byte to the whole eax register
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_GT:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("cmp %%ebx, %%eax\n");
                GEN_ASM("setg %%al\n");        // Set the lower byte of the eax register to 1 if the comparison is true
                GEN_ASM("movzx %%al, %%eax\n");// Move the lower byte to the whole eax register
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_AND:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                // eax and ebx are bools (0 or 1), so we can use the and instruction
                GEN_ASM("and %%ebx, %%eax\n");
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_OR:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                srcIntoRegister(t->src[1], output_file, "ebx", current_function);
                GEN_ASM("or %%ebx, %%eax\n");
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_NOT:
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                // eax is a bool (0 or 1), so we can use the not instruction and then and it with 1
                GEN_ASM("not %%eax\n");
                GEN_ASM("and $1, %%eax\n");
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                break;
            case TAC_IFZ:
                // If the src is zero, jump to the label
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                GEN_ASM("cmp $0, %%eax\n");
                GEN_ASM("je %s\n", t->src[1]->value);
                break;
            case TAC_BEGFUN:
                current_function = t->src[0];
                COMMENT("Function %s", current_function->value);
                COMMENT("Function parameters offsets:")
                setFunctionParametersOffsets(t->src[0], output_file);
                GEN_ASM("%s:\n", t->src[0]->value);
                COMMENT("Function prologue")
                GEN_ASM("push %%ebp\n");
                GEN_ASM("mov %%esp, %%ebp\n");
                // TODO: Check if the function has local variables to allocate
                // Save the registers that will be used
                GEN_ASM("push %%ebx\n");
                GEN_ASM("push %%esi\n");
                GEN_ASM("push %%edi\n");
                COMMENT("Function body")
                break;
            case TAC_ENDFUN:
                COMMENT("Function epilogue")
                // Pop the registers that were used
                GEN_ASM("pop %%edi\n");
                GEN_ASM("pop %%esi\n");
                GEN_ASM("pop %%ebx\n");
                // TODO: Check if the function has local variables to deallocate
                // Deallocate the local variables
                GEN_ASM("mov %%ebp, %%esp\n");
                // Restore the previous frame pointer
                GEN_ASM("pop %%ebp\n");
                // Return
                GEN_ASM("ret\n\n\n");
                current_function = NULL;
                break;
            case TAC_ARG:
                // Nenhum argumento é passado por registrador
                // Os argumentos são passados na pilha
                // Nenhum registrador é salvo antes da chamada
                // Os argumentos são passados da direita para a esquerda
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                GEN_ASM("push %%eax\n");
                break;
            case TAC_CALL: {
                // Get the arguments size
                int args_size = t->src[0]->identifier->type->arg_list->arg_count * 4;
                // Get the register saving spot
                int register_saving_spot = args_size + 12;
                // Save the registers that will be used in the place reserved for them
                GEN_ASM("mov %%ebx, -%d(%%ebp)\n", register_saving_spot);
                GEN_ASM("mov %%esi, -%d(%%ebp)\n", register_saving_spot - 4);
                GEN_ASM("mov %%edi, -%d(%%ebp)\n", register_saving_spot - 8);

                GEN_ASM("call %s\n", t->src[0]->value);
                // The return value is in the eax register
                GEN_ASM("mov %%eax, %s(,1)\n", t->dst->value);
                // Pop the arguments from the stack
                GEN_ASM("add $%d, %%esp\n", args_size);
                // Pop the registers that were used
                GEN_ASM("pop %%edi\n");
                GEN_ASM("pop %%esi\n");
                GEN_ASM("pop %%ebx\n");

                break;
            }
            case TAC_RET:
                // Move the return value to the eax register
                srcIntoRegister(t->src[0], output_file, "eax", current_function);
                // Pop the registers that were used
                GEN_ASM("pop %%edi\n");
                GEN_ASM("pop %%esi\n");
                GEN_ASM("pop %%ebx\n");
                // TODO: Check if the function has local variables to deallocate
                // Deallocate the local variables
                GEN_ASM("mov %%ebp, %%esp\n");
                // Restore the previous frame pointer
                GEN_ASM("pop %%ebp\n");
                // Return
                GEN_ASM("ret\n");
                break;
            case TAC_PRINT:
                if (t->src[0]->type == SYMBOL_LIT_STRING) {
                    GEN_ASM("push $%s\n", t->src[0]->value);
                    GEN_ASM("call printf\n");
                    GEN_ASM("add $4, %%esp\n");
                } else {
                    srcIntoRegister(t->src[0], output_file, "eax", current_function);
                    GEN_ASM("push %%eax\n");
                    HashEntry *format_string = format_string_int;
                    GEN_ASM("push $%s\n", format_string->value);
                    GEN_ASM("call printf\n");
                    GEN_ASM("add $8, %%esp\n");
                }
                break;
            case TAC_READ:
                // Call scanf with the address of the variable as the argument
                GEN_ASM("push $%s\n", t->dst->value);
                GEN_ASM("push $%s\n", scan_format_string_int->value);
                GEN_ASM("call scanf\n");
                GEN_ASM("add $8, %%esp\n");
                // Check EAX for errors, if EAX is 0, there was an error reading the input
                GEN_ASM("cmp $0, %%eax\n");
                GEN_ASM("je __scanf_error\n");
                break;
            case TAC_MOVE_ARRAY:
                // Move value to array
                // Syntax: MOVE_ARRAY src0: index, src1: value, dst: array
                srcIntoRegister(t->src[0], output_file, "eax", current_function);// Index
                srcIntoRegister(t->src[1], output_file, "edx", current_function);// Value
                GEN_ASM("mov $%s, %%ebx\n", t->dst->value);                      // Array
                GEN_ASM("mov %%edx, (%%ebx, %%eax, 4)\n");
                // TODO: Check the type of the array for the size

                break;
            case TAC_LOAD_ARRAY:
                // Load value from array
                // Syntax: LOAD_ARRAY src0: array, src1: index, dst: value
                GEN_ASM("mov $%s, %%ebx\n", t->src[0]->value);                   // Array
                srcIntoRegister(t->src[1], output_file, "eax", current_function);// Index
                GEN_ASM("mov (%%ebx, %%eax, 4), %%edx\n");
                GEN_ASM("mov %%edx, %s(,1)\n", t->dst->value);
                // TODO: Check if the destination is a parameter

                break;
            case TAC_MAX:
                ERROR("Invalid TAC operation MAX");
                break;
            case TAC_COMMENT:
                COMMENT("%s", t->src[0]->value);
                break;
            case TAC_CONSUMED_TEMP:
                // TODO:
                break;
            case TAC_WILL_CALL:
                COMMENT("Function call %s", t->src[0]->value);
                // Create space for the registers that will be saved
                GEN_ASM("sub $12, %%esp\n");// 3 registers * 4 bytes
                break;
        }
    }
}
void setFunctionParametersOffsets(HashEntry *fun, FILE *output_file) {
    // Iterate over the function parameters
    ParamTypeList *param_list = fun->identifier->type->arg_list;
    ParamIterator *paramIterator = newParamIterator(param_list);
    int offset = 8;// The first parameter is at offset 8
    if (paramIteratorDone(paramIterator)) {
        COMMENT("No parameters");
    }
    while (!paramIteratorDone(paramIterator)) {
        ParamType *param = getNextParam(paramIterator);
        COMMENT("%s %s at %d", getTypeBaseName(param->type->base), param->symbol->value, offset);
        GEN_ASM(".set %s_at_%s, %d\n", param->symbol->value, fun->value, offset);
        // TODO: Calculate the offset based on the type size
        offset += 4;
    }

    destroyParamIterator(paramIterator);
}


void generateAssembly(TacList *code, AST *ast, FILE *output_file) {
    generateVariableDeclarations(code, ast, output_file);
    generateFunctions(code, output_file);

    // Add the scanf error handler
    GEN_ASM("__scanf_error:\n");
    GEN_ASM("push $%s\n", scan_error_string->value);
    GEN_ASM("call printf\n");
    GEN_ASM("add $4, %%esp\n");
    GEN_ASM("push $1\n");
    GEN_ASM("call exit\n");
    GEN_ASM("add $4, %%esp\n");

    // Postamble
}
