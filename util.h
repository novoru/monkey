#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdnoreturn.h>
#include <stdarg.h>
#include <string.h>

#define DEBUG 1

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))
#define DEBUG_PRINT(fmt, args...)						\
  do { if (DEBUG) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt,		\
			  __FILE__, __LINE__, __func__, ##args); } while (0)

noreturn void error(char *fmt, ...) __attribute__((format(printf, 1, 2)));
char *format(char *fmt, ...) __attribute__((format(printf, 1, 2)));

char *substr(char *s, int from, int len);
char *ctos(char c);

// Vector
typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void vec_del(Vector *vec);

// Map
typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
void map_del(Map *map);



#endif
