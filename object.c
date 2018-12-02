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

char *inspect_obj(Object *obj) {
  switch (obj->ty) {
  case OBJ_INT:
    return format("%d", (int)obj->value);
  case OBJ_BOOL:
    return format("%s", ((_Bool)obj->value ? "true" : "false"));
  case OBJ_NULL:
    return "NULL";
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
  }
  return "";
}
