#ifndef AST_H
#define AST_H

#include "token.h"
#include "util.h"

enum {
      AST_STMT,       // statement
      AST_IDENT,      // identifier
      AST_EXPR,       // expression
      AST_INT,        // integer
      AST_LET,        // let statement
      AST_RETURN,     // return statement
      AST_EXPR_STMT,  // expression statement
      AST_PREF_EXPR,  // prefix expression
      AST_INF_EXPR,   // infix expression
      AST_BOOL,       // boolean
      AST_IF_EXPR,    // if expression
      AST_BLOCK_STMT, // block statement
};

typedef struct Node {
  int ty;
  Token *token;
  struct Node *ident;   // identifier
  char *name;           // name of identifier
  struct Node *expr;    // expression
  long value;           // value of integer literal
  char *op;             // operator
  struct Node *left;    // left hand side
  struct Node *right;   // right hand side
  _Bool bool;           // True:1 or False:0
  struct Node *cond;    // condition
  struct Node *conseq;  // concequence
  struct Node *alter;   // alternative
  Vector *stmts;        // statements of block statement
} Node;

typedef struct {
  Vector *stmts;  //statements 
} Program;

Node *new_stmt(Token *token);
Node *new_ident(Token *token);
Node *new_expr(Token *token);
Node *new_let_stmt(Token *token);
Node *new_return_stmt(Token *token);
Node *new_expr_stmt(Token *token, Node *expr);
Node *new_int_expr(Token *token);
Node *new_pref_expr(Token *token, char *op);
Node *new_inf_expr(Token *token, char *op, Node *left);
Node *new_bool(Token *token);
Node *new_if_expr(Token *token);
Node *new_block_stmt(Token *token);
void del_node(Node *node);
Program *new_program();
void del_program(Program *program);
char *program_to_str(Program *program);
char *node_to_str(Node *node);
char *node_type(int ty);

#endif
