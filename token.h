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

      TOK_ASSIGN,
      TOK_PLUS,

      TOK_COMMA,
      TOK_SEMICOLON,

      TOK_LPAREN,
      TOK_RPAREN,
      TOK_LBRACE,
      TOK_RBRACE,

      TOK_FUNCTION,
      TOK_LET,
};

Map *keywords;
Token *new_token(int ty, char *lit);
void new_keywords();
void del_token(Token *tok);
int lookup_ident(char *ident);

#endif
