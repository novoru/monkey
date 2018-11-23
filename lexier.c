#include "lexier.h"

Lexier *new_lexier(char *input) {
  Lexier *l = malloc(sizeof(Lexier));
  l->input = malloc(strlen(input)+1);
  l->pos = 0;
  l->rpos = 0;
  strcpy(l->input, input);
  read_char(l);

  new_keywords();
  
  return l;
}

void del_lexier(Lexier *l) {
  free(l->input);
  free(l);
}

void read_char(Lexier *l) {
  if (l->rpos >= strlen(l->input))
    l->ch = '\0';
  else
    l->ch = l->input[l->rpos];
  
  l->pos = l->rpos;
  l->rpos += 1;

}

char *read_ident(Lexier *l) {
  int pos = l->pos;
  while (isalpha(l->ch) || l->ch == '_')
    read_char(l);
  return substr(l->input, pos, l->pos-pos);
}

char *read_num(Lexier *l) {
  int pos = l->pos;
  while (isdigit(l->ch))
    read_char(l);

  return substr(l->input, pos, l->pos-pos);
}

void skip(Lexier *l) {
  while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r')
    read_char(l);
}

char peek_char(Lexier *l) {
  if (l->rpos >= strlen(l->input))
    return 0;
  else
    return l->input[l->rpos];
}

Token *next_token_lexier(Lexier *l) {
  Token *tok;

  skip(l);

  switch (l->ch) {
  case '=':
    if (peek_char(l) == '=') {
      read_char(l);
      tok = new_token(TOK_EQ, "==");
    }
    else
      tok = new_token(TOK_ASSIGN, "=");
    break;
  case ';':
    tok = new_token(TOK_SEMICOLON, ";");
    break;
  case '(':
    tok = new_token(TOK_LPAREN, "(");
    break;
  case ')':
    tok = new_token(TOK_RPAREN, ")");
    break;
  case ',':
    tok = new_token(TOK_COMMA, ",");
    break;
  case '+':
    tok = new_token(TOK_PLUS, "+");
    break;
  case '-':
    tok = new_token(TOK_MINUS, "-");
    break;
  case '!':
    if (peek_char(l) == '=') {
      read_char(l);
      tok = new_token(TOK_NOT_EQ, "!=");
    }
    else
      tok = new_token(TOK_BANG, "!");
    break;
  case '/':
    tok = new_token(TOK_SLASH, "/");
    break;
  case '*':
    tok = new_token(TOK_ASTERISK, "*");
    break;
  case '<':
    tok = new_token(TOK_LT, "<");
    break;
  case '>':
    tok = new_token(TOK_GT, ">");
    break;
  case '{':
    tok = new_token(TOK_LBRACE, "{");
    break;
  case '}':
    tok = new_token(TOK_RBRACE, "}");
    break;
  case '\0':
    tok = new_token(TOK_EOF, "");
    break;
  default:
    if (isalpha(l->ch) || l->ch == '_') {
      char *lit = read_ident(l);
      int ty = lookup_ident(lit);
      tok = new_token(ty, lit);
      return tok;
    }
    else if(isdigit(l->ch)) {
      char *lit = read_num(l);
      int ty = TOK_INT;
      tok = new_token(ty, lit);
      return tok;
    }
    else
      tok = new_token(TOK_ILLEGAL, ctos(l->ch));
  }

  read_char(l);

  return tok;
}

