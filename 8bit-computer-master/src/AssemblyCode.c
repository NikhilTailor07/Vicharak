#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// AST Node Types (from the parser)
typedef enum
{
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_EXPRESSION,
    NODE_IF,
    NODE_BLOCK,
    NODE_UNKNOWN
} NodeType;

// AST Node Structure
typedef struct ASTNode
{
    NodeType type;
    char text[100];
    struct ASTNode *children[3]; // Max 3 children
    int child_count;
} ASTNode;

// Symbol Table Entry
typedef struct
{
    char name[100];
    int address;
} Symbol;

// Symbol Table
Symbol symbol_table[100];
int symbol_count = 0;

// Function Prototypes
void generateCode(ASTNode *node);
void emit(const char *instruction);
int getSymbolAddress(const char *name);
void addSymbol(const char *name);

// Emit assembly code
void emit(const char *instruction)
{
    printf("%s\n", instruction);
}

// Get the memory address of a variable
int getSymbolAddress(const char *name)
{
    for (int i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i].name, name) == 0)
        {
            return symbol_table[i].address;
        }
    }
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    exit(1);
}

// Add a variable to the symbol table
void addSymbol(const char *name)
{
    symbol_table[symbol_count].address = symbol_count; // Assign address incrementally
    strcpy(symbol_table[symbol_count].name, name);
    symbol_count++;
}

// Generate code for the AST
void generateCode(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case NODE_PROGRAM:
        for (int i = 0; i < node->child_count; i++)
        {
            generateCode(node->children[i]);
        }
        break;

    case NODE_VAR_DECL:
        addSymbol(node->text); // Add variable to symbol table
        // No assembly needed for declaration
        break;

    case NODE_ASSIGN:
    {
        int address = getSymbolAddress(node->text); // Variable being assigned
        generateCode(node->children[0]);            // Generate code for expression
        char instruction[50];
        sprintf(instruction, "STORE R0, %d", address); // Store result in memory
        emit(instruction);
        break;
    }

    case NODE_EXPRESSION:
    {
        if (node->child_count == 1)
        {
            // Single operand (e.g., a number or variable)
            if (isdigit(node->children[0]->text[0]))
            {
                char instruction[50];
                sprintf(instruction, "LOADI R0, %s", node->children[0]->text); // Load immediate value
                emit(instruction);
            }
            else
            {
                int address = getSymbolAddress(node->children[0]->text);
                char instruction[50];
                sprintf(instruction, "LOAD R0, %d", address); // Load variable value
                emit(instruction);
            }
        }
        else if (node->child_count == 3)
        {
            // Binary operation (e.g., a + b)
            generateCode(node->children[0]); // Left operand
            emit("PUSH R0");                 // Push result onto stack
            generateCode(node->children[2]); // Right operand
            emit("POP R1");                  // Pop left operand into R1

            char instruction[50];
            if (strcmp(node->children[1]->text, "+") == 0)
            {
                sprintf(instruction, "ADD R0, R1");
            }
            else if (strcmp(node->children[1]->text, "-") == 0)
            {
                sprintf(instruction, "SUB R0, R1");
            }
            else
            {
                fprintf(stderr, "Error: Unsupported operator '%s'\n", node->children[1]->text);
                exit(1);
            }
            emit(instruction);
        }
        break;
    }

    case NODE_IF:
    {
        generateCode(node->children[0]); // Condition
        emit("CMP R0, 1");               // Compare condition result to true (1)
        char label[20];
        static int label_count = 0;
        sprintf(label, "LABEL_%d", label_count++);
        char instruction[50];
        sprintf(instruction, "JNE %s", label); // Jump if condition is false
        emit(instruction);

        generateCode(node->children[1]); // If block
        emit(label);                     // Label for the end of the if block
        break;
    }

    case NODE_BLOCK:
        for (int i = 0; i < node->child_count; i++)
        {
            generateCode(node->children[i]);
        }
        break;

    default:
        fprintf(stderr, "Error: Unknown node type\n");
        exit(1);
    }
}
