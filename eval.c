#include "eval.h"

Object *eval_stmts(Vector *stmts) {
  Object *result;
  
  for (int i = 0; i < stmts->len; i++) {
    result = eval((Node *)stmts->data[i]);
  }

  return result;
}

Object *eval(Node *node) {
  switch (node->ty) {
  case AST_PROGRAM:
    return eval_stmts(node->stmts);
  case AST_EXPR_STMT:
    return eval(node->expr);
  case AST_INT:
    return new_int_obj(node->value);
  }
  return NULL;
}
