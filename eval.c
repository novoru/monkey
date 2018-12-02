#include "eval.h"

Object *get_bool_obj(_Bool bool){
  if (bool) {
    if(true_obj == NULL) true_obj = new_bool_obj(true);
    return true_obj;
  }
  if (false_obj == NULL) false_obj = new_bool_obj(false);
  return false_obj;
}

Object *get_null_obj() {
  if (null_obj == NULL)
    null_obj = new_null_obj();
  return null_obj;
}

Object *eval_stmts(Vector *stmts) {
  Object *result;
  
  for (int i = 0; i < stmts->len; i++) {
    result = eval((Node *)stmts->data[i]);
  }

  return result;
}

Object *eval_bang_op_expr(Object *right) {
  switch (right->ty) {
  case OBJ_BOOL:
    if((_Bool)right->value)
      return get_bool_obj(false);
    else
      return get_bool_obj(true);
  case OBJ_NULL:
    return get_bool_obj(true);
  default:
    return get_bool_obj(false);
  }
}

Object *eval_pref_expr(char *op, Object *right) {
  if (!strcmp(op, "!"))
    return eval_bang_op_expr(right);
  else if (!strcmp(op, "-"))
    return NULL;
  else
    return NULL;
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
    return get_bool_obj((_Bool)node->bool);
  case AST_PREF_EXPR:
    goto workaround;
  workaround:;
    Object *right = eval(node->right);
    return eval_pref_expr(node->op, right);
  }
  return NULL;
}
