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
  case AST_BOOL:
    return new_bool_obj(node->bool);
  }
  return NULL;
}

Object *bool_obj(Node *node){
  if (node->bool == true) {
    if(true_obj == NULL) true_obj = new_bool_obj(true);
    return true_obj;
  }
  if (false_obj == NULL) false_obj = new_bool_obj(false);
  return false_obj;
}
