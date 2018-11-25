#include "ast.h"
#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *new_stmt(Token *token) {
  Node *stmt = malloc(sizeof(Node));
  stmt->ty = AST_STMT;
  stmt->token = token;

  return stmt;
}

Node *new_ident(Token *token) {
  Node *ident = malloc(sizeof(Node));
  ident->ty = AST_IDENT;
  ident->token = token;
  ident->name = token->lit;
  
  return ident;
}

Node *new_expr(Token *token) {
  Node *expr = malloc(sizeof(Node));
  expr->ty = AST_EXPR;
  expr->token = token;
  
  return expr;
}

Node *new_let_stmt(Token *token) {
  Node *let_stmt = malloc(sizeof(Node));
  let_stmt->ty = AST_LET;
  let_stmt->token = token;

  return let_stmt;
  
}

Node *new_return_stmt(Token *token) {
  Node *return_stmt = malloc(sizeof(Node));
  return_stmt->ty = AST_RETURN;
  return_stmt->token = token;

  return return_stmt;
  
}

Node *new_expr_stmt(Token *token, Node *expr) {
  Node *expr_stmt = malloc(sizeof(Node));
  expr_stmt->ty = AST_EXPR_STMT;
  expr_stmt->token = token;
  expr_stmt->expr = expr;

  return expr_stmt;
}

Node *new_int_expr(Token *token) {
  Node *lit = malloc(sizeof(Node));
  lit->ty = AST_INT;
  lit->token = token;

  return lit;
}

Node *new_pref_expr(Token *token, char *op) {
  Node *pref_expr = malloc(sizeof(Node));

  pref_expr->token = token;
  pref_expr->ty = AST_PREF_EXPR;
  pref_expr->op = op;
  
  return pref_expr;
}

Node *new_inf_expr(Token *token, char *op, Node *left) {
  Node *inf_expr = malloc(sizeof(Node));
  inf_expr->token = token;
  inf_expr->ty = AST_INF_EXPR;
  inf_expr->op = op;
  inf_expr->left = left;

  return inf_expr;
}

Node *new_bool(Token *token) {
  Node *boolean = malloc(sizeof(Node));
  boolean->ty = AST_BOOL;
  boolean->token = token;

  return boolean;
}

void del_node(Node *node) {
  if (node == NULL) return;
  switch (node->ty) {
  case AST_IDENT:
    break;
  case AST_INT:
    break;
  case AST_EXPR_STMT:
    del_node(node->expr);
    break;
  case AST_PREF_EXPR:
    del_node(node->right);
    break;
  case AST_INF_EXPR:
    del_node(node->left);
    del_node(node->right);
    break;
  }
  free(node);
}

Program *new_program() {
  Program *program = malloc(sizeof(Program));
  program->stmts = new_vector();
  
  return program;
}

void del_program(Program *program) {
  vec_del(program->stmts);
  free(program);
}

char *program_to_str(Program *program) {
  if (program->stmts->len < 1)
    return NULL;

  char *tmp = node_to_str((Node *)program->stmts->data[0]);
  char *str = malloc(sizeof(tmp));
  str = strcpy(str, tmp);

  for(int i = 1;i < program->stmts->len; i++) {
    tmp = node_to_str((Node *)program->stmts->data[i]);
    str = realloc(str, sizeof(str)+sizeof(tmp));
    strcat(str, tmp);
  }

  return str;
  
}

char *node_to_str(Node *node) {
  char *str;

  switch (node->ty) {
  case AST_IDENT:
    return node->name;
  case AST_LET:
    str = malloc(sizeof(node->token->lit)+2);
    strcpy(str, node->token->lit);
    strcat(str, " ");
    if (node->ident != NULL) {
      str = realloc(str, sizeof(str)+sizeof(node->ident->name)+3);
      strcat(str, node->ident->name);
      strcat(str, " = ");
    }
    if (node->expr != NULL) {
      char *tmp = node_to_str(node->expr);
      str = realloc(str, sizeof(str)+sizeof(tmp));
      strcat(str, tmp);
    }
    strcat(str, ";");
    return str;
  case AST_RETURN:
    str = malloc(sizeof(node->token->lit)+2);
    strcpy(str, node->token->lit);
    strcat(str, " ");
     if (node->expr != NULL) {
      char *tmp = node_to_str(node->expr);
      str = realloc(str, sizeof(str)+sizeof(tmp));
      strcat(str, tmp);
    }
    strcat(str, ";");
    return str;
  case AST_EXPR_STMT:
    if (node->expr != NULL)
      return node_to_str(node->expr);
    return "";
  case AST_EXPR:
    return "";
  case AST_PREF_EXPR:
    return format("(%s%s)", node->op, node_to_str(node->right));
  case AST_INF_EXPR:
    return format("(%s %s %s)",node_to_str(node->left), node->op, node_to_str(node->right));
  case AST_BOOL:
    return node->token->lit;
  case AST_INT:
    return format("%d", node->value);
  }
  
  return NULL;
}

char *node_type(int ty) {
  switch (ty) {
  case AST_STMT:
    return "AST_STMT";
  case AST_IDENT:
    return "AST_IDENT";
  case AST_EXPR:
    return "AST_EXPR";
  case AST_INT:
    return "AST_INT";
  case AST_LET:
    return "AST_LET";
  case AST_RETURN:
    return "AST_RETURN";
  case AST_EXPR_STMT:
    return "AST_EXPR_STMT";
  case AST_PREF_EXPR:
    return "AST_PREF_EXPR";
  case AST_INF_EXPR:
    return "AST_INF_EXPR";
  case AST_BOOL:
    return "AST_BOOL";
  }
}
