#include "parser.h"
#include "util.h"

#include <ctype.h>

Parser *new_parser(Lexier *l) {
  Parser *parser = malloc(sizeof(Parser));
  parser->lexier = l;
  parser->cur_token = NULL;
  parser->peek_token = NULL;
  parser->errors = new_vector();
  
  next_token_parser(parser);
  next_token_parser(parser);

  new_precedences();
  
  pref_parse_funcs = new_map();
  register_pref(TOK_IDENT,    parse_ident);
  register_pref(TOK_INT,      parse_int_expr);
  register_pref(TOK_BANG,     parse_pref_expr);
  register_pref(TOK_MINUS,    parse_pref_expr);
  register_pref(TOK_TRUE,     parse_boolean);
  register_pref(TOK_FALSE,    parse_boolean);  
  register_pref(TOK_LPAREN,   parse_grouped_expr);
  register_pref(TOK_IF,       parse_if_expr);
  register_pref(TOK_FUNCTION, parse_func);
  
  inf_parse_funcs = new_map();
  register_inf(TOK_PLUS,      parse_inf_expr);
  register_inf(TOK_MINUS,     parse_inf_expr);
  register_inf(TOK_SLASH,     parse_inf_expr);
  register_inf(TOK_ASTERISK,  parse_inf_expr);
  register_inf(TOK_EQ,        parse_inf_expr);
  register_inf(TOK_NOT_EQ,    parse_inf_expr);
  register_inf(TOK_LT,        parse_inf_expr);
  register_inf(TOK_GT,        parse_inf_expr);
  
  return parser;
}

void del_parser(Parser *parser) {
  del_token(parser->cur_token);
  del_token(parser->peek_token);
  vec_del(parser->errors);
  map_del(pref_parse_funcs);
  map_del(inf_parse_funcs);
  free(parser);
}

void next_token_parser(Parser *parser){
  parser->cur_token = parser->peek_token;
  parser->peek_token = next_token_lexier(parser->lexier);

}

Node *parse_stmt(Parser *parser){
  switch (parser->cur_token->ty) {
  case TOK_LET:
    return parse_let_stmt(parser);
  case TOK_RETURN:
    return parse_return_stmt(parser);
  default:
    return parse_expr_stmt(parser);
  }
}

Node *parse_let_stmt(Parser *parser) {
  Node *stmt = new_let_stmt(parser->cur_token);

  if (expect_peek(parser, TOK_IDENT) == 0)
    return NULL;
  
  stmt->ident = new_ident(parser->cur_token);

  while (parser->cur_token->ty != TOK_SEMICOLON)
    next_token_parser(parser);

  return stmt;
}

Node *parse_return_stmt(Parser *parser) {
  Node *stmt = new_return_stmt(parser->cur_token);

  next_token_parser(parser);

  while (parser->cur_token->ty != TOK_SEMICOLON)
    next_token_parser(parser);

  return stmt;  
}

Node *parse_expr(Parser *parser, int precedence) {
  Node* (*pref)(Parser *) = map_get(pref_parse_funcs,
				    token_type(parser->cur_token->ty));
  if (pref == NULL) {
    no_pref_parse_func_err(parser, parser->cur_token->ty);
    return NULL;
  }

  Node *left = (Node *)pref(parser);
  
  while ((parser->peek_token->ty != TOK_SEMICOLON) &&
	 (precedence < peek_precedence(parser))) {
    Node* (*infix)(Parser *, Node *) = map_get(inf_parse_funcs,
					       token_type(parser->peek_token->ty));
    if (infix == NULL)
      return left;

    next_token_parser(parser);

    left = infix(parser, left);
    
  }

  return left;
}

Node *parse_expr_stmt(Parser *parser) {
  Node *stmt = new_expr_stmt(parser->cur_token,
			     parse_expr(parser, PREC_LOWEST));

  if (parser->peek_token->ty == TOK_SEMICOLON)
    next_token_parser(parser);
  
  return stmt;
}

Node *parse_ident(Parser *parser) {
  Node *ident = new_ident(parser->cur_token);
  
  return ident;
}

Node *parse_int_expr(Parser *parser) {
  Node *lit = new_int_expr(parser->cur_token);

  // TODO: error handling
  /*
  if (isdigit(*parser->cur_token->literal) == 0) {
    char *fmt = "could not parse %s as integer";
    size_t needed = snprintf(NULL, 0, fmt, parser->cur_token->literal);
    char *msg = malloc(sizeof(needed));
    sprintf(msg, fmt, parser->cur_token->literal);
    vec_push(parser->errors, (char *)msg);
    return NULL;
  }
  */

  lit->value = strtol(parser->cur_token->lit, NULL, 10);

  return lit;
}

Node *parse_pref_expr(Parser *parser) {
  Node *expr = new_pref_expr(parser->cur_token,
			     parser->cur_token->lit);
  next_token_parser(parser);
  expr->right = parse_expr(parser, PREC_PREFIX);

  return expr;
}

Node *parse_inf_expr(Parser *parser, Node *left) {
  Node *expr = new_inf_expr(parser->cur_token,
			    parser->cur_token->lit,
			    left);

  int prec = cur_precedence(parser);
  next_token_parser(parser);
  expr->right =parse_expr(parser, prec);

  return expr;
}

Node *parse_boolean(Parser *parser) {
  Node *bool = new_bool(parser->cur_token);
  bool->bool = (strcmp(parser->cur_token->lit, "true") == 0) ? 1 : 0;
  
  return bool;
}

Node *parse_grouped_expr(Parser *parser) {
  next_token_parser(parser);

  Node *expr = parse_expr(parser, PREC_LOWEST);

  if (!expect_peek(parser, TOK_RPAREN))
    return NULL;

  return expr;
}

Node *parse_if_expr(Parser *parser) {
  Node *expr = new_if_expr(parser->cur_token);
  if (!expect_peek(parser, TOK_LPAREN))
    return NULL;

  next_token_parser(parser);
  expr->cond = parse_expr(parser, PREC_LOWEST);

  if (!expect_peek(parser, TOK_RPAREN))
    return NULL;

  if (!expect_peek(parser, TOK_LBRACE))
    return NULL;

  expr->conseq = parse_block_stmt(parser);

  if (peek_token_is(parser, TOK_ELSE)) {
    next_token_parser(parser);

    if (!expect_peek(parser, TOK_LBRACE))
      return NULL;

    expr->alter = parse_block_stmt(parser);
  }
  
  return expr;
}

Node *parse_block_stmt(Parser *parser) {
  Node *block = new_block_stmt(parser->cur_token);

  next_token_parser(parser);

  while(!cur_token_is(parser, TOK_RBRACE) && !cur_token_is(parser, TOK_EOF)) {
    Node *stmt = parse_stmt(parser);
    if (stmt != NULL)
      vec_push(block->stmts, (void *)stmt);
    next_token_parser(parser);
  }

  return block;
    
}

Node *parse_func(Parser *parser) {
  Node *func = new_func(parser->cur_token);

  if (!expect_peek(parser, TOK_LPAREN))
    return NULL;

  func->params = parse_func_params(parser);

  if (!expect_peek(parser, TOK_LBRACE))
    return NULL;

  func->body = parse_block_stmt(parser);

  return func;
}

Vector *parse_func_params(Parser *parser) {
  Vector *params = new_vector();

  if (peek_token_is(parser, TOK_RPAREN)) {
    next_token_parser(parser);
    return params;
  }

  next_token_parser(parser);

  Node *param = new_ident(parser->cur_token);
  vec_push(params, (void *)param);

  while (peek_token_is(parser, TOK_COMMA)) {
    next_token_parser(parser);
    next_token_parser(parser);
    param = new_ident(parser->cur_token);
    vec_push(params, (void *)param);
  }

  if (!expect_peek(parser, TOK_RPAREN))
    return NULL;

  return params;
}

Program *parse_program(Parser *parser) {
  Program *program = new_program();
  Node *stmt;
  
  while (parser->cur_token->ty != TOK_EOF) {
    stmt = parse_stmt(parser);
    if (stmt != NULL) 
      vec_push(program->stmts, (Node *)stmt);
    next_token_parser(parser);
  }
  
  return program;
}

void peek_error(Parser *parser, int ty) {
  char *err = format("expected next token to be %s, got %s instead",
		     token_type(ty), token_type(parser->peek_token->ty));
  vec_push(parser->errors, err);
}

_Bool cur_token_is(Parser *parser, int ty) {
  return parser->cur_token->ty == ty;
}

_Bool peek_token_is(Parser *parser, int ty) {
  return parser->peek_token->ty == ty;
}

_Bool expect_peek(Parser *parser, int ty) {
  if (peek_token_is(parser, ty)) {
    next_token_parser(parser);
    return true;
  }
  else
    return false;
}

void register_pref(int ty, void(*func)) {
  map_put(pref_parse_funcs, token_type(ty), func);
}

void register_inf(int ty, void(*func)) {
  map_put(inf_parse_funcs, token_type(ty), func);
}

void no_pref_parse_func_err(Parser *parser, int ty) {
  char *msg = format("no prefix parse function for %s found",
		     token_type(ty));
  vec_push(parser->errors, (char *)msg);
}

int peek_precedence(Parser *parser) {
  int prec = (int)map_get(precedences, token_type(parser->peek_token->ty));
  if (prec != NULL)
    return prec;
  
  return PREC_LOWEST;
}

int cur_precedence(Parser *parser) {
  int prec = (int)map_get(precedences, token_type(parser->cur_token->ty));
  if (prec != NULL)
    return prec;
  
  return PREC_LOWEST;
}

void new_precedences() {
  precedences = new_map();
  map_put(precedences, token_type(TOK_EQ),       (void *)PREC_EQUALS); 
  map_put(precedences, token_type(TOK_NOT_EQ),   (void *)PREC_EQUALS); 
  map_put(precedences, token_type(TOK_LT),       (void *)PREC_LESSGREATER); 
  map_put(precedences, token_type(TOK_GT),       (void *)PREC_LESSGREATER); 
  map_put(precedences, token_type(TOK_PLUS),     (void *)PREC_SUM); 
  map_put(precedences, token_type(TOK_MINUS),    (void *)PREC_SUM); 
  map_put(precedences, token_type(TOK_SLASH),    (void *)PREC_PRODUCT); 
  map_put(precedences, token_type(TOK_ASTERISK), (void *)PREC_PRODUCT); 
}
