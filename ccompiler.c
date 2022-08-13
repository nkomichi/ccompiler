#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Kind of tokens
typedef enum {
    TK_RESERVED,  // Operators
    TK_NUM,       // Integers
    TK_EOF,       // End Of File
} TokenKind;

typedef struct Token Token;

// Token structure
struct Token {
    TokenKind kind;  // Kind of token
    Token *next;     // Next input token
    int val;         // Number if kind is TK_NUM
    char *str;       // String of token
};

// Token in interest
Token *token;

// Function to report error
// same arguments as printf
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// If next token is expected operator, go to next token
// and return true. Else, return false.
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// If next token is expected operator, go to next token.
// Else, report error.
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error("Operator is not '%c'", op);
    token = token->next;
}

// If next token is number, go to next token and return the number.
// Else, report error.
int expect_number() {
    if (token->kind != TK_NUM)
        error("Not a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// Create new token and connect to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// Return tokenized input string p
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // Skip space
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        error("Could not tokenize");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Number of arguments are invalid\n");
        return 1;
    }

    // Tokenize
    token = tokenize(argv[1]);

    // First half of assembly
    printf("    .globl _main\n");
    printf("    .p2align 2\n");
    printf("_main:\n");

    // Initial token should be a number
    printf("    mov x0, #%d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add w0, w0, #%d\n", expect_number());
            continue;
        }
        expect('-');
        printf("    sub w0, w0, #%d\n", expect_number());
    }

    printf("    ret\n");
    return 0;
}