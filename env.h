#ifndef ENV_H
#define ENV_H

#include "object.h"
#include "util.h"

typedef struct Env {
  Map *store;
  struct Env *outer;
} Env;

Env *new_env();
Env *new_enclosed_env();
Object *get_env(Env *env, char *name);
Object *set_env(Env *env, char *name, Object *val);
char *inspect_env(Env *env);

#endif
