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

Object *eval_minus_pref_expr(Object *right) {
  if (right->ty != OBJ_INT)
    return get_null_obj();

  long value = (long)right->value;

  return new_int_obj(-value);
}

Object *eval_pref_expr(char *op, Object *right) {
  if (!strcmp(op, "!"))
    return eval_bang_op_expr(right);
  else if (!strcmp(op, "-"))
    return eval_minus_pref_expr(right);
  else
    return get_null_obj();
}

Object *eval_int_inf_expr(char *op, Object *left, Object *right) {
  long lval = (long)left->value;
  long rval = (long)right->value;

  if (!strcmp(op, "+"))
    return new_int_obj(lval+rval);
  else if(!strcmp(op, "-"))
    return new_int_obj(lval-rval);
  else if(!strcmp(op, "*"))
    return new_int_obj(lval*rval);
  else if(!strcmp(op, "/"))
    return new_int_obj(lval/rval);
  else if(!strcmp(op, "<"))
    return get_bool_obj(lval < rval);
  else if(!strcmp(op, ">"))
    return get_bool_obj(lval > rval);
  else if(!strcmp(op, "=="))
    return get_bool_obj(lval == rval);
  else if(!strcmp(op, "!="))
    return get_bool_obj(lval != rval);
  else
    return get_null_obj();
  
}

Object *eval_inf_expr(char *op, Object *left, Object *right) {
  if (left->ty == OBJ_INT && right->ty == OBJ_INT)
    return eval_int_inf_expr(op, left, right);
  else if (!strcmp(op, "=="))
    return get_bool_obj((_Bool)left->value == (_Bool)right->value);
  else if (!strcmp(op, "!="))
    return get_bool_obj((_Bool)left->value != (_Bool)right->value);
  return get_null_obj();
}

_Bool is_truthy(Object *obj) {
  switch (obj->ty) {
  case OBJ_NULL:
    return false;
  case OBJ_BOOL:
    return (_Bool)obj->value;
  default:
    return true;
  }
}

Object *eval_if_expr(Node *ie) {
  Object *cond = eval(ie->cond);

  if (is_truthy(cond))
    return eval(ie->conseq);
  else if (ie->alter != NULL)
    return eval(ie->alter);
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
    goto pref_workaround;
  pref_workaround:;
    Object *pref_right = eval(node->right);
    return eval_pref_expr(node->op, pref_right);
  case AST_INF_EXPR:
    goto inf_workaround;
  inf_workaround:;
    Object *inf_left = eval(node->left);
    Object *inf_right = eval(node->right);
    return eval_inf_expr(node->op, inf_left, inf_right);
  case AST_BLOCK_STMT:
    return eval_stmts(node->stmts);
  case AST_IF_EXPR:
    return eval_if_expr(node);
  }
  return get_null_obj();
}
