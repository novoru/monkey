#include "eval.h"

static Object *get_bool_obj(_Bool bool){
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

static Object *new_error(char *fmt, ...) {
  char *buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  return new_error_obj(strdup(buf));
}

static _Bool is_error(Object *obj) {
  if (obj != NULL)
    return obj->ty == OBJ_ERROR;

  return false;
}

static Object *eval_stmts(Vector *stmts, Env *env) {
  Object *result;
  
  for (int i = 0; i < stmts->len; i++) {
    result = eval((Node *)stmts->data[i], env);

    if (result->ty == OBJ_RETURN)
      return (Object *)result->value;
  }

  return result;
}

static Object *eval_bang_op_expr(Object *right) {
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

static Object *eval_minus_pref_expr(Object *right) {
  if (right->ty != OBJ_INT)
    return new_error("unknown operator: -%s", obj_type(right));

  long value = (long)right->value;

  return new_int_obj(-value);
}

static Object *eval_pref_expr(char *op, Object *right) {
  if (!strcmp(op, "!"))
    return eval_bang_op_expr(right);
  else if (!strcmp(op, "-"))
    return eval_minus_pref_expr(right);
  else
    return new_error("unkown operator: %s%s", op, obj_type(right));
}

static Object *eval_int_inf_expr(char *op, Object *left, Object *right) {
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
    return new_error("unknown operator: %s %s %s", obj_type(left), op, obj_type(right));
  
}

static Object *eval_inf_expr(char *op, Object *left, Object *right) {
  if (left->ty == OBJ_INT && right->ty == OBJ_INT)
    return eval_int_inf_expr(op, left, right);
  else if (!strcmp(op, "=="))
    return get_bool_obj((_Bool)left->value == (_Bool)right->value);
  else if (!strcmp(op, "!="))
    return get_bool_obj((_Bool)left->value != (_Bool)right->value);
  else if(left->ty != right->ty)
    return new_error("type mismatch: %s %s %s", obj_type(left), op, obj_type(right));
  return new_error("unknown operator: %s %s %s", obj_type(left), op, obj_type(right));
}

static _Bool is_truthy(Object *obj) {
  switch (obj->ty) {
  case OBJ_NULL:
    return false;
  case OBJ_BOOL:
    return (_Bool)obj->value;
  default:
    return true;
  }
}

static Object *eval_if_expr(Node *ie, Env *env) {
  Object *cond = eval(ie->cond, env);

  if (is_error(cond))
    return cond;

  if (is_truthy(cond))
    return eval(ie->conseq, env);
  else if (ie->alter != NULL)
    return eval(ie->alter, env);
  else
    return get_null_obj();
}

static Object *eval_ident(Node *node, Env *env) {
  Object *val = (Object *)get_env(env, node->name);

  if (val == NULL)
    return new_error("identifier not found: %s", node->name);

  return val;
}

static Object *eval_program(Node *node, Env *env) {
  Object *result;

  for ( int i = 0; i < node->stmts->len; i++) {
    Node *stmt = (Node *)node->stmts->data[i];
    result = eval(stmt, env);

    if (result->ty == OBJ_RETURN)
      return (Object *)result->value;
    else if(result->ty == OBJ_ERROR)
      return result;
  }

  return result;
}

static Object *eval_block_stmt(Node *block, Env *env) {
  Object *result;

  for ( int i = 0; i < block->stmts->len; i++) {
    Node *stmt = (Node *)block->stmts->data[i];
    result = eval(stmt, env);

    if (result != NULL && (result->ty == OBJ_RETURN || result->ty == OBJ_ERROR))
      return result;
  }

  return result;
}

static Vector *eval_exprs(Vector *args, Env *env) {
  Vector *result = new_vector();

  for (int i = 0; i < args->len; i++) {
    Object *evaluated = eval((Node *)args->data[i], env);
    if (is_error(evaluated)) {
      result = new_vector();
      vec_push(result, evaluated);
      return result;
    }
    vec_push(result, evaluated);
  }

  return result;
}

static Object *unwrap_ret_val(Object *obj) {
  if (obj->ty == OBJ_RETURN)
    return (Object *)obj->value;

  return obj;
}

static Env *extend_func_env(Object *fn, Vector *args) {
  Env *env = new_enclosed_env(fn->env);

  for (int i = 0; i < args->len; i++) {
    Node *param = (Node *)fn->params->data[i];
    Object *arg = (Object *)args->data[i];
    set_env(env, (char *)param->name, arg);
  }

  return env;
}

static Object *apply_func(Object *fn, Vector *args) {
  if (fn->ty != OBJ_FUNCTION)
    return new_error_obj(format("not a function: %s", obj_type(fn)));

  Env *extended_env = extend_func_env(fn, args);
  Object *evaluated = eval(fn->body, extended_env);

  return unwrap_ret_val(evaluated);
}

Object *eval(Node *node, Env *env) {
  switch (node->ty) {
  case AST_PROGRAM:
    return eval_program(node, env);
  case AST_EXPR_STMT:
    return eval(node->expr, env);
  case AST_INT:
    return new_int_obj(node->value);
  case AST_BOOL:
    return get_bool_obj((_Bool)node->bool);
  case AST_PREF_EXPR:
    ;   // workaround
    Object *pref_right = eval(node->right, env);
    if (is_error(pref_right))
      return pref_right;
    return eval_pref_expr(node->op, pref_right);
  case AST_INF_EXPR:
    ;   // workaround
    Object *inf_left = eval(node->left, env);
    if (is_error(inf_left))
      return inf_left;
    Object *inf_right = eval(node->right, env);
    if (is_error(inf_right))
      return inf_right;
    return eval_inf_expr(node->op, inf_left, inf_right);
  case AST_BLOCK_STMT:
    if (node->stmts->len > 0)
      return eval_block_stmt(node, env);
    return get_null_obj();
  case AST_IF_EXPR:
    return eval_if_expr(node, env);
  case AST_RETURN:
    ;   // workaround
    Object *ret_val = eval(node->ret, env);
    if (is_error(ret_val))
      return ret_val;
    return new_return_obj(ret_val);
  case AST_LET:
    ;
    Object *let_val = eval(node->data, env);
    if (is_error(let_val))
      return let_val;
    return set_env(env, node->ident->name, let_val);
  case AST_IDENT:
    return eval_ident(node, env);
  case AST_FUNCTION:
    ;   // workaround
    Vector *params = node->params;
    Node *body = node->body;
    return new_func_obj(params, env, body);
  case AST_CALL_EXPR:
    ;   // workaround
    Object *func = eval(node->func, env);
    if (is_error(func))
      return func;
    Vector *args = eval_exprs(node->args, env);
    if (args->len == 1 && is_error((Object *)args->data[0]))
      return (Object *)args->data[0];
    return apply_func(func, args);
  }
  return get_null_obj();
}
