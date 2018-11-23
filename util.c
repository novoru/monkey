#include "util.h"

noreturn void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

char *format(char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  
  return strdup(buf);
}

char *substr(char *s, int from, int len) {
  if (len > strlen(s) || len <= 0)
    return NULL;
  char *sub = malloc((len+1) * sizeof(char));
  memcpy(sub, &s[from], len);
  sub[len] = '\0';

  return strdup(sub);
}

char *ctos(char c) {
  char s[2];
  s[0] = c;
  s[1] = '\0';

  return strdup(s);
}

// Vector

Vector *new_vector() {
  Vector *vec = malloc(sizeof(Vector));
  vec->data = malloc(sizeof(void *) * 16);
  vec->capacity = 16;
  vec->len = 0;
  return vec;
}

void vec_push(Vector *vec, void *elem) {
  if (vec->capacity == vec->len) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
  }
  vec->data[vec->len++] = elem;
}

void vec_del(Vector *vec) {
  free(vec->data);
  free(vec);
}

// Map

Map *new_map() {
  Map *map = malloc(sizeof(Map));
  map->keys = new_vector();
  map->vals = new_vector();
  return map;
}

void map_put(Map *map, char *key, void *val) {
  vec_push(map->keys, key);
  vec_push(map->vals, val);
}

void *map_get(Map *map, char *key) {
  for (int i = map->keys->len -1; i >= 0; i--) {
    if (strcmp(map->keys->data[i], key) == 0)
      return map->vals->data[i];
  }
  return NULL;
}

void map_del(Map *map) {
  vec_del(map->keys);
  vec_del(map->vals);
  free(map);
}
