#ifndef ENV_H
#define ENV_H

#include "object.h"
#include "util.h"

typedef struct Env {
  Map *store;
} Env;

Env *new_env();
Object *get_env(Env *env, char *name);
Object *set_env(Env *env, char *name, Object *val);

#endif
