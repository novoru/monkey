#include "eval.h"
#include "object.h"

Object *new_int_obj(long value) {
  Object *o = malloc(sizeof(Object));
  o->ty = OBJ_INT;
  o->value = (void *)value;

  return o;
}

Object *new_bool_obj(_Bool value) {
  Object *o = malloc(sizeof(Object));
  o->ty = OBJ_BOOL;
  o->value = (void *)value;

  return o;
}

Object *new_null_obj() {
  Object *o = malloc(sizeof(Object));
  o->ty = OBJ_NULL;
  o->value = NULL;

  return o;
}

Object *new_return_obj(Object *value) {
  Object *o = malloc(sizeof(Object));
  o->ty = OBJ_RETURN;
  o->value = (void *)value;

  return o;
}

Object *new_error_obj(char *msg) {
  Object *o = malloc(sizeof(Object));
  o->ty = OBJ_ERROR;
  o->value = (void *)msg;
  
  return o;
}

Object *new_func_obj(Vector *params, Env *env, Node *body) {
  Object *o = malloc(sizeof(Object));
  o->ty = OBJ_FUNCTION;
  o->params = params;
  o->env = env;
  o->body = body;

  return o;
}

char *inspect_obj(Object *obj) {
  switch (obj->ty) {
  case OBJ_INT:
    return format("%d", (int)obj->value);
  case OBJ_BOOL:
    return format("%s", ((_Bool)obj->value ? "true" : "false"));
  case OBJ_NULL:
    return "NULL";
  case OBJ_RETURN:
    return inspect_obj((Object *)obj->value);
  case OBJ_ERROR:
    return (char *)obj->value;
  case OBJ_FUNCTION:
    ;    // workaround
    char *str = format("fn(");
    for (int i = 0; i < obj->params->len; i++) {
      Node *param = (Node *)obj->params->data[i];
      if (i == 0)
	str = format("%s%s", str, param->name);
      else
	str = format("%s, %s", str, param->name);
    }
    return format("%s) {\n%s\n}", str, node_to_str(obj->body));
  }

  return "";
}

char *obj_type(Object *obj) {
  switch (obj->ty) {
  case OBJ_INT:
    return "OBJ_INT";
  case OBJ_BOOL:
    return "OBJ_BOOL";
  case OBJ_NULL:
    return "OBJ_NULL";
  case OBJ_RETURN:
    return "OBJ_RETURN";
  case OBJ_ERROR:
    return "OBJ_ERROR";
  case OBJ_FUNCTION:
    return "OBJ_FUNCTION";
  }
  return "";
}
