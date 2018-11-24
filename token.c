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
    return "TOK_ILLEGAL";
    break;
  case TOK_EOF:
    return "TOK_EOF";
    break;
  case TOK_IDENT:
    return "TOK_IDENT";
    break;
  case TOK_INT:
    return "TOK_INT";
    break;
  case TOK_ASSIGN:
    return "TOK_ASSIGN";
    break;
  case TOK_PLUS:
    return "TOK_PLUS";
    break;
  case TOK_MINUS:
    return "TOK_MINUS";
    break;
  case TOK_BANG:
    return "TOK_BANG";
    break;
  case TOK_ASTERISK:
    return "TOK_ASTERISK";
    break;
  case TOK_SLASH:
    return "TOK_SLASH";
    break;
  case TOK_LT:
    return "TOK_LT";
    break;
  case TOK_GT:
    return "TOK_GT";
    break;
  case TOK_COMMA:
    return "TOK_COMMA";
    break;
  case TOK_SEMICOLON:
    return "TOK_SEMICOLON";
    break;
  case TOK_LPAREN:
    return "TOK_LPAREN";
    break;
  case TOK_RPAREN:
    return "TOK_RPAREN";
    break;
  case TOK_LBRACE:
    return "TOK_LBRACE";
    break;
  case TOK_RBRACE:
    return "TOK_RBRACE";
    break;
  case TOK_FUNCTION:
    return "TOK_FUNCTION";
    break;
  case TOK_LET:
    return "TOK_LET";
    break;
  case TOK_TRUE:
    return "TOK_TRUE";
    break;
  case TOK_FALSE:
    return "TOK_FALSE";
    break;
  case TOK_IF:
    return "TOK_IF";
    break;
  case TOK_ELSE:
    return "TOK_ELSE";
    break;
  case TOK_RETURN:
    return "TOK_RETURN";
    break;
  case TOK_EQ:
    return "TOK_EQ";
    break;
  case TOK_NOT_EQ:
    return "TOK_NOT_EQ";
    break;
  }
}
