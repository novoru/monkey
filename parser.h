#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexier.h"
#include "token.h"
#include "util.h"

// precedence
enum {
  PREC__,
  PREC_LOWEST,
  PREC_EQUALS,           // ==
  PREC_LESSGREATER,      // > or <
  PREC_SUM,              // +
  PREC_PRODUCT,          // *
  PREC_PREFIX,           // -X or !X
  PREC_CALL,             // myFunction(X)
};

typedef struct {
  Lexier *lexier;
  Token *cur_token;
  Token *peek_token;
  Vector *errors;
} Parser;

Map *pref_parse_funcs;
Map *inf_parse_funcs;
Map *precedences;

Parser *new_parser(Lexier *lexier);
void del_parser(Parser *parser);
void next_token_parser(Parser *parser);
Program *parse_program(Parser *parser);
Node *parse_stmt(Parser *parser);
Node *parse_let_stmt(Parser *parser);
Node *parse_return_stmt(Parser *parser);
Node *parse_expr(Parser *parser, int precedence);
Node *parse_expr_stmt(Parser *parser);
Node *parse_ident(Parser *parser);
Node *parse_int_expr(Parser *parser);
Node *parse_pref_expr(Parser *parser);
Node *parse_inf_expr(Parser *parser, Node *left);
Node *parse_boolean(Parser *parser);
Node *parse_grouped_expr(Parser *parser);
Node *parse_if_expr(Parser *parser);
Node *parse_block_stmt(Parser *parser);
Node *parse_func(Parser *parser);
Vector *parse_func_params(Parser *parser);
Program *parse_program(Parser *parser);
void peek_error(Parser *parser, int ty);

_Bool cur_token_is(Parser *parser, int ty);
_Bool peek_token_is(Parser *parser, int ty);
_Bool expect_peek(Parser *parser, int ty);

void register_pref(int ty, void(*func));
void register_inf(int ty, void(*func));
void no_pref_parse_func_err(Parser *parser, int ty);
int peek_precedence(Parser *parser);
int cur_precedence(Parser *parser);
void new_precedences();

#endif
