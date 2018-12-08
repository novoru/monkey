#include "env.h"

Env *new_env() {
  Env *env = malloc(sizeof(Env));
  env->store = new_map();
  
  return env;
}

Object *get_env(Env *env, char *name) {
  if (map_get(env->store, name) != NULL)
    return (Object *)map_get(env->store, name);

  return get_null_obj();
}

Object *set_env(Env *env, char *name, Object *val) {
  map_put(env->store, name, (void *)val);
  return val;
}
