#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHILDREN 3

// Token Types (from the lexer)
typedef enum
{
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_IF,
    TOKEN_EQUAL,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

typedef struct
{
    TokenType type;
    char text[100];
} Token;

// AST Node Types
typedef enum
{
    NODE_PROGRAM,    // Root node
    NODE_VAR_DECL,   // Variable declaration
    NODE_ASSIGN,     // Assignment
    NODE_EXPRESSION, // Arithmetic expression
    NODE_IF,         // If condition
    NODE_BLOCK,      // Code block
    NODE_UNKNOWN     // Unknown node
} NodeType;

// AST Node
typedef struct ASTNode
{
    NodeType type;
    char text[100];                         // Optional, e.g., variable name
    struct ASTNode *children[MAX_CHILDREN]; // Child nodes
    int child_count;                        // Number of children
} ASTNode;

// Global Token
Token current_token;

// Function Prototypes
ASTNode *parseProgram(FILE *file);
ASTNode *parseStatement(FILE *file);
ASTNode *parseVarDecl(FILE *file);
ASTNode *parseAssignment(FILE *file);
ASTNode *parseExpression(FILE *file);
ASTNode *parseIf(FILE *file);
void getNextToken(FILE *file, Token *token);
void error(const char *message);
ASTNode *createNode(NodeType type, const char *text);
void printAST(ASTNode *node, int level);
void freeAST(ASTNode *node);

// Lexer Function (from the previous lexer implementation)
void getNextToken(FILE *file, Token *token);

// Error Handling
void error(const char *message)
{
    printf("Syntax Error: %s\n", message);
    exit(1);
}

// Create a new AST node
ASTNode *createNode(NodeType type, const char *text)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    if (text)
        strcpy(node->text, text);
    node->child_count = 0;
    return node;
}

// Add a child to an AST node
void addChild(ASTNode *parent, ASTNode *child)
{
    if (parent->child_count < MAX_CHILDREN)
        parent->children[parent->child_count++] = child;
    else
        error("Exceeded maximum number of child nodes");
}

// Parse the entire program
ASTNode *parseProgram(FILE *file)
{
    ASTNode *root = createNode(NODE_PROGRAM, "Program");
    while (current_token.type != TOKEN_EOF)
    {
        ASTNode *stmt = parseStatement(file);
        addChild(root, stmt);
    }
    return root;
}

// Parse a statement
ASTNode *parseStatement(FILE *file)
{
    if (current_token.type == TOKEN_INT)
    {
        return parseVarDecl(file);
    }
    else if (current_token.type == TOKEN_IDENTIFIER)
    {
        return parseAssignment(file);
    }
    else if (current_token.type == TOKEN_IF)
    {
        return parseIf(file);
    }
    else
    {
        error("Unexpected token in statement");
    }
    return NULL;
}

// Parse a variable declaration
ASTNode *parseVarDecl(FILE *file)
{
    getNextToken(file); // Consume 'int'
    if (current_token.type != TOKEN_IDENTIFIER)
    {
        error("Expected variable name after 'int'");
    }
    ASTNode *node = createNode(NODE_VAR_DECL, current_token.text);
    getNextToken(file); // Consume identifier
    if (current_token.type != TOKEN_SEMICOLON)
    {
        error("Expected ';' after variable declaration");
    }
    getNextToken(file); // Consume ';'
    return node;
}

// Parse an assignment
ASTNode *parseAssignment(FILE *file)
{
    ASTNode *node = createNode(NODE_ASSIGN, current_token.text);
    getNextToken(file); // Consume identifier
    if (current_token.type != TOKEN_ASSIGN)
    {
        error("Expected '=' in assignment");
    }
    getNextToken(file); // Consume '='
    ASTNode *expr = parseExpression(file);
    addChild(node, expr);
    if (current_token.type != TOKEN_SEMICOLON)
    {
        error("Expected ';' after assignment");
    }
    getNextToken(file); // Consume ';'
    return node;
}

// Parse an expression (currently supports simple arithmetic)
ASTNode *parseExpression(FILE *file)
{
    ASTNode *node = createNode(NODE_EXPRESSION, NULL);
    if (current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_NUMBER)
    {
        addChild(node, createNode(NODE_UNKNOWN, current_token.text));
        getNextToken(file); // Consume identifier/number
        if (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS)
        {
            addChild(node, createNode(NODE_UNKNOWN, current_token.text));
            getNextToken(file); // Consume operator
            if (current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_NUMBER)
            {
                addChild(node, createNode(NODE_UNKNOWN, current_token.text));
                getNextToken(file); // Consume identifier/number
            }
            else
            {
                error("Expected identifier or number after operator");
            }
        }
    }
    else
    {
        error("Invalid expression");
    }
    return node;
}

// Parse an if statement
ASTNode *parseIf(FILE *file)
{
    ASTNode *node = createNode(NODE_IF, "if");
    getNextToken(file); // Consume 'if'
    if (current_token.type != TOKEN_LBRACE)
    {
        error("Expected '{' after 'if'");
    }
    getNextToken(file); // Consume '{'
    ASTNode *block = createNode(NODE_BLOCK, "Block");
    while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF)
    {
        addChild(block, parseStatement(file));
    }
    if (current_token.type != TOKEN_RBRACE)
    {
        error("Expected '}' at the end of if block");
    }
    getNextToken(file); // Consume '}'
    addChild(node, block);
    return node;
}

// Print the AST for debugging
void printAST(ASTNode *node, int level)
{
    for (int i = 0; i < level; i++)
        printf("  ");
    printf("Node Type: %d, Text: %s\n", node->type, node->text);
    for (int i = 0; i < node->child_count; i++)
    {
        printAST(node->children[i], level + 1);
    }
}

// Free the AST
void freeAST(ASTNode *node)
{
    for (int i = 0; i < node->child_count; i++)
    {
        freeAST(node->children[i]);
    }
    free(node);
}

// Main function
int main()
{
    FILE *file = fopen("input.txt", "r");
    if (!file)
    {
        perror("Failed to open input file");
        return 1;
    }

    getNextToken(file, &current_token); // Initialize the first token
    ASTNode *root = parseProgram(file);
    printAST(root, 0);
    freeAST(root);

    fclose(file);
    return 0;
}
