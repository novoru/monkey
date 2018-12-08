#ifndef OBJECT_H
#define OBJECT_H

#include "ast.h"
#include "env.h"
#include "util.h"

enum {
      OBJ_INT,
      OBJ_BOOL,
      OBJ_NULL,
      OBJ_RETURN,
      OBJ_ERROR,
      OBJ_FUNCTION,
};

typedef struct Object{
  int ty;
  void *value;
  Vector *params;   // paramters of function
  Node *body;       // body of function
  struct Env *env;
} Object;

Object *true_obj;
Object *false_obj;
Object *null_obj;

Object *new_int_obj(long value);
Object *new_bool_obj(_Bool value);
Object *new_null_obj();
Object *new_return_obj(Object *obj);
Object *new_error_obj(char *msg);
Object *new_func_obj(Vector *params, struct Env *env, Node *body);
char *inspect_obj(Object *obj);
char *obj_type(Object *obj);

#endif
