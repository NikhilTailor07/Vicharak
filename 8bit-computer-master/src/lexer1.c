#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100

// Enum to represent token types
typedef enum
{
    TOKEN_INT,        // "int" keyword
    TOKEN_IF,         // "if" keyword
    TOKEN_IDENTIFIER, // Variable names
    TOKEN_NUMBER,     // Numeric literals
    TOKEN_ASSIGN,     // "="
    TOKEN_PLUS,       // "+"
    TOKEN_MINUS,      // "-"
    TOKEN_EQUAL,      // "=="
    TOKEN_LBRACE,     // "{"
    TOKEN_RBRACE,     // "}"
    TOKEN_SEMICOLON,  // ";"
    TOKEN_UNKNOWN,    // Unknown character
    TOKEN_EOF         // End of file
} TokenType;

// Struct to represent a token
typedef struct
{
    TokenType type;           // Type of the token
    char text[MAX_TOKEN_LEN]; // The actual token text
} Token;

// Function to create the next token
void getNextToken(FILE *file, Token *token)
{
    int c;

    // Skip whitespace
    while ((c = fgetc(file)) != EOF)
    {
        if (isspace(c))
            continue;

        // Check for keywords or identifiers
        if (isalpha(c))
        {
            int len = 0;
            token->text[len++] = c;
            while (isalnum(c = fgetc(file)))
            {
                if (len < MAX_TOKEN_LEN - 1)
                    token->text[len++] = c;
            }
            ungetc(c, file); // Push back the last read character
            token->text[len] = '\0';

            // Check for keywords
            if (strcmp(token->text, "int") == 0)
            {
                token->type = TOKEN_INT;
            }
            else if (strcmp(token->text, "if") == 0)
            {
                token->type = TOKEN_IF;
            }
            else
            {
                token->type = TOKEN_IDENTIFIER; // Otherwise, it's an identifier
            }
            return;
        }

        // Check for numbers
        if (isdigit(c))
        {
            int len = 0;
            token->text[len++] = c;
            while (isdigit(c = fgetc(file)))
            {
                if (len < MAX_TOKEN_LEN - 1)
                    token->text[len++] = c;
            }
            ungetc(c, file); // Push back the last read character
            token->text[len] = '\0';
            token->type = TOKEN_NUMBER;
            return;
        }

        // Handle single-character tokens and operators
        switch (c)
        {
        case '=':
            if ((c = fgetc(file)) == '=')
            {
                token->type = TOKEN_EQUAL;
                strcpy(token->text, "==");
            }
            else
            {
                ungetc(c, file);
                token->type = TOKEN_ASSIGN;
                strcpy(token->text, "=");
            }
            return;
        case '+':
            token->type = TOKEN_PLUS;
            strcpy(token->text, "+");
            return;
        case '-':
            token->type = TOKEN_MINUS;
            strcpy(token->text, "-");
            return;
        case '{':
            token->type = TOKEN_LBRACE;
            strcpy(token->text, "{");
            return;
        case '}':
            token->type = TOKEN_RBRACE;
            strcpy(token->text, "}");
            return;
        case ';':
            token->type = TOKEN_SEMICOLON;
            strcpy(token->text, ";");
            return;
        default:
            token->type = TOKEN_UNKNOWN;
            token->text[0] = c;
            token->text[1] = '\0';
            return;
        }
    }

    // End of file
    token->type = TOKEN_EOF;
    token->text[0] = '\0';
}

// Main function to test the lexer
int main()
{
    FILE *file = fopen("input.txt", "r");
    if (!file)
    {
        perror("Failed to open input file");
        return 1;
    }

    Token token;
    printf("Tokens:\n");
    do
    {
        getNextToken(file, &token);
        printf("Type: %d, Text: %s\n", token.type, token.text);
    } while (token.type != TOKEN_EOF);

    fclose(file);
    return 0;
}
