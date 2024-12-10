#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token Types
typedef enum
{
    TOKEN_INT,
    TOKEN_IF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_EQUAL,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

// Token Structure
typedef struct
{
    TokenType type;
    char text[100];
} Token;

// AST Node Types
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

// Global Variables
Token tokens[100];
int token_index = 0;
int token_count = 0;
int symbol_count = 0;
int current_token_index = 0;
Token current_token;

// Function Prototypes
void lexer(const char *input);
Token getNextToken();
ASTNode *parseProgram();
ASTNode *parseStatement();
ASTNode *parseExpression();
void generateCode(ASTNode *node);
void emit(const char *instruction);
void testCompiler(const char *inputProgram);

// Emit assembly code
void emit(const char *instruction)
{
    printf("%s\n", instruction);
}

// Lexer: Tokenize the input
void lexer(const char *input)
{
    int len = strlen(input);
    int i = 0;
    while (i < len)
    {
        char c = input[i];
        if (isspace(c))
        {
            i++;
            continue;
        }
        Token token = {TOKEN_UNKNOWN, ""};
        if (isalpha(c))
        {
            int start = i;
            while (isalnum(input[i]))
                i++;
            strncpy(token.text, input + start, i - start);
            token.text[i - start] = '\0';
            if (strcmp(token.text, "int") == 0)
                token.type = TOKEN_INT;
            else if (strcmp(token.text, "if") == 0)
                token.type = TOKEN_IF;
            else
                token.type = TOKEN_IDENTIFIER;
        }
        else if (isdigit(c))
        {
            int start = i;
            while (isdigit(input[i]))
                i++;
            strncpy(token.text, input + start, i - start);
            token.text[i - start] = '\0';
            token.type = TOKEN_NUMBER;
        }
        else
        {
            switch (c)
            {
            case '=':
                token.type = TOKEN_ASSIGN;
                strcpy(token.text, "=");
                break;
            case '+':
                token.type = TOKEN_PLUS;
                strcpy(token.text, "+");
                break;
            case '-':
                token.type = TOKEN_MINUS;
                strcpy(token.text, "-");
                break;
            case '{':
                token.type = TOKEN_LBRACE;
                strcpy(token.text, "{");
                break;
            case '}':
                token.type = TOKEN_RBRACE;
                strcpy(token.text, "}");
                break;
            case ';':
                token.type = TOKEN_SEMICOLON;
                strcpy(token.text, ";");
                break;
            default:
                fprintf(stderr, "Unknown character: %c\n", c);
                exit(1);
            }
            i++;
        }
        tokens[token_count++] = token;
    }
    tokens[token_count++] = (Token){TOKEN_EOF, ""};
}

// Get the next token
Token getNextToken()
{
    if (current_token_index < token_count)
    {
        return tokens[current_token_index++];
    }
    return (Token){TOKEN_EOF, ""};
}

// Parser: Parse a program
ASTNode *parseProgram()
{
    ASTNode *program = malloc(sizeof(ASTNode));
    program->type = NODE_PROGRAM;
    program->child_count = 0;
    while (current_token_index < token_count)
    {
        ASTNode *stmt = parseStatement();
        if (stmt)
        {
            program->children[program->child_count++] = stmt;
        }
        else
        {
            break;
        }
    }
    return program;
}

// Parser: Parse a statement
ASTNode *parseStatement()
{
    Token token = getNextToken();
    if (token.type == TOKEN_INT)
    {
        Token var = getNextToken();
        if (var.type == TOKEN_IDENTIFIER)
        {
            Token semi = getNextToken();
            if (semi.type == TOKEN_SEMICOLON)
            {
                ASTNode *node = malloc(sizeof(ASTNode));
                node->type = NODE_VAR_DECL;
                strcpy(node->text, var.text);
                node->child_count = 0;
                return node;
            }
        }
    }
    else if (token.type == TOKEN_IDENTIFIER)
    {
        Token assign = getNextToken();
        if (assign.type == TOKEN_ASSIGN)
        {
            ASTNode *expr = parseExpression();
            Token semi = getNextToken();
            if (semi.type == TOKEN_SEMICOLON)
            {
                ASTNode *node = malloc(sizeof(ASTNode));
                node->type = NODE_ASSIGN;
                strcpy(node->text, token.text);
                node->children[0] = expr;
                node->child_count = 1;
                return node;
            }
        }
    }
    else if (token.type == TOKEN_IF)
    {
        Token lbrace = getNextToken();
        if (lbrace.type == TOKEN_LBRACE)
        {
            ASTNode *block = parseProgram();
            Token rbrace = getNextToken();
            if (rbrace.type == TOKEN_RBRACE)
            {
                ASTNode *node = malloc(sizeof(ASTNode));
                node->type = NODE_IF;
                node->children[0] = block;
                node->child_count = 1;
                return node;
            }
        }
    }
    return NULL;
}

// Parser: Parse an expression
ASTNode *parseExpression()
{
    Token token = getNextToken();
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_EXPRESSION;
    strcpy(node->text, token.text);
    node->child_count = 0;
    return node;
}

// Test the compiler
void testCompiler(const char *inputProgram)
{
    lexer(inputProgram);
    ASTNode *ast = parseProgram();
    printf("Generated Assembly Code:\n");
    generateCode(ast);
}

int main()
{
    const char *inputProgram =
        "int a;\n"
        "a = 10;\n"
        "if { a = 20; }\n";
    testCompiler(inputProgram);
    return 0;
}
