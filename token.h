#ifndef TOKEN_H
#define TOKEN_H

#include "util.h"

typedef struct Token {
  int ty;
  char *lit;
} Token;

enum {
      TOK_ILLEGAL,
      TOK_EOF,
      
      TOK_IDENT,
      TOK_INT,

      TOK_ASSIGN,     // '='
      TOK_PLUS,       // '+'
      TOK_MINUS,      // '-'
      TOK_BANG,       // '!'
      TOK_ASTERISK,   // '*'
      TOK_SLASH,      // '/'

      TOK_LT,         // '<'
      TOK_GT,         // '>'

      TOK_COMMA,      // ','
      TOK_SEMICOLON,  // ';'

      TOK_LPAREN,     // '('
      TOK_RPAREN,     // ')'
      TOK_LBRACE,     // '{'
      TOK_RBRACE,     // '}'

      TOK_FUNCTION,   // 'fn'
      TOK_LET,        // 'let'
      TOK_TRUE,       // 'true'
      TOK_FALSE,      // 'false'
      TOK_IF,         // 'if'
      TOK_ELSE,       // 'else'
      TOK_RETURN,     // 'return'

      TOK_EQ,         // '=='
      TOK_NOT_EQ      // '!='
};

Map *keywords;
Token *new_token(int ty, char *lit);
void new_keywords();
void del_token(Token *tok);
int lookup_ident(char *ident);
char *token_type(int ty);

#endif
