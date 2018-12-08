#include "env.h"

Env *new_env() {
  Env *env = malloc(sizeof(Env));
  env->store = new_map();
  
  return env;
}

Env *new_enclosed_env(Env *outer) {
  Env *env = new_env();
  env->outer = outer;

  return env;
}

Object *get_env(Env *env, char *name) {
  Object *obj = (Object *)map_get(env->store, name);
  if (obj == NULL && env->outer != NULL)
    return obj = map_get(env->outer->store, name);

  return obj;
}

Object *set_env(Env *env, char *name, Object *val) {
  map_put(env->store, name, (void *)val);
  return val;
}

char *inspect_env(Env *env) {
  if (env->store == NULL)
    return "";
  char *str = "";
  if (env->outer != NULL)
    str = format("%s\n", inspect_env(env->outer));
  str = format("%s{ ", str);
  for (int i = 0; i < env->store->keys->len; i++) {
    Object *val = env->store->vals->data[i];
    if (i == 0)
      str = format("%s%s: %s", str, (char *)env->store->keys->data[i], inspect_obj(val));
    else
      str = format("%s, %s: %s", str, (char *)env->store->keys->data[i], inspect_obj(val));
  }
  return format("%s }", str);
}
