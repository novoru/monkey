#ifndef OBJECT_H
#define OBJECT_H

#include "ast.h"
#include "util.h"

enum {
      OBJ_INT,
      OBJ_BOOL,
      OBJ_NULL,
};

typedef struct Object{
  int ty;
  void *value;
} Object;

Object *true_obj;
Object *false_obj;

Object *new_int_obj(long value);
Object *new_bool_obj(_Bool value);
Object *new_null_obj();
char *inspect_obj(Object *obj);
char *obj_type(Object *obj);

#endif
