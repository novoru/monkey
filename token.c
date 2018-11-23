#include "token.h"

Token *new_token(int ty, char *lit) {
  Token *tok = malloc(sizeof(Token));
  tok->ty = ty;
  tok->lit = malloc(strlen(lit));
  strcpy(tok->lit, lit);

  return tok;
}

void new_keywords() {
  keywords = new_map();
  map_put(keywords, "fn",     (void *)TOK_FUNCTION);
  map_put(keywords, "let",    (void *)TOK_LET);
  map_put(keywords, "true",   (void *)TOK_TRUE);
  map_put(keywords, "false",  (void *)TOK_FALSE);
  map_put(keywords, "if",     (void *)TOK_IF);
  map_put(keywords, "else",   (void *)TOK_ELSE);
  map_put(keywords, "return", (void *)TOK_RETURN);
}

void del_token(Token *tok) {
  free(tok->lit);
  free(tok);
}

int lookup_ident(char *ident) {
  int ty = (int)(__intptr_t) map_get(keywords, ident);
  if (ty)
    return ty;
  return TOK_IDENT;
}

char *token_type(int ty) {
  switch (ty) {
  case TOK_ILLEGAL:
    return "ILLEGAL";
    break;
  case TOK_EOF:
    return "EOF";
    break;
  case TOK_IDENT:
    return "IDENT";
    break;
  case TOK_INT:
    return "INT";
    break;
  case TOK_ASSIGN:
    return "ASSIGN";
    break;
  case TOK_PLUS:
    return "PLUS";
    break;
  case TOK_MINUS:
    return "MINUS";
    break;
  case TOK_BANG:
    return "BANG";
    break;
  case TOK_ASTERISK:
    return "ASTERISK";
    break;
  case TOK_SLASH:
    return "SLASH";
    break;
  case TOK_LT:
    return "LT";
    break;
  case TOK_GT:
    return "GT";
    break;
  case TOK_COMMA:
    return "COMMA";
    break;
  case TOK_SEMICOLON:
    return "SEMICOLON";
    break;
  case TOK_LPAREN:
    return "LPAREN";
    break;
  case TOK_RPAREN:
    return "RPAREN";
    break;
  case TOK_LBRACE:
    return "LBRACE";
    break;
  case TOK_RBRACE:
    return "RBRACE";
    break;
  case TOK_FUNCTION:
    return "FUNCTION";
    break;
  case TOK_LET:
    return "LET";
    break;
  case TOK_TRUE:
    return "TRUE";
    break;
  case TOK_FALSE:
    return "FALSE";
    break;
  case TOK_IF:
    return "IF";
    break;
  case TOK_ELSE:
    return "ELSE";
    break;
  case TOK_RETURN:
    return "RETURN";
    break;
  case TOK_EQ:
    return "EQ";
    break;
  case TOK_NOT_EQ:
    return "NOT_EQ";
    break;
  }
}
