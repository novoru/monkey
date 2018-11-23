#ifndef LEXIER_H
#define LEXIER_H

#include "token.h"
#include "util.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct Lexier {
  char *input;
  int pos;
  int rpos;
  char ch;
} Lexier;

Lexier *new_lexier(char *input);
void del_lexier(Lexier *l);
void read_char(Lexier *l);
char *read_ident(Lexier *l);

Token *next_token_lexier(Lexier *l);

#endif
