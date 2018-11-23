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
