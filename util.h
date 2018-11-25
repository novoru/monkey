#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdnoreturn.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1
#define true  1
#define false 0

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))
#define DEBUG_PRINT(fmt, args...)						\
  do { if (DEBUG) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt,		\
			  __FILE__, __LINE__, __func__, ##args); } while (0)

enum t_typename {
  TYPENAME_BOOL,
  TYPENAME_UNSIGNED_CHAR,
  TYPENAME_CHAR,
  TYPENAME_SIGNED_CHAR,
  TYPENAME_SHORT_INT,
  TYPENAME_UNSIGNED_SHORT_INT,
  TYPENAME_INT,
  TYPENAME_UNSIGNED_INT,
  TYPENAME_LONG_INT,
  TYPENAME_UNSIGNED_LONG_INT,
  TYPENAME_LONG_LONG_INT,
  TYPENAME_UNSIGNED_LONG_LONG_INT,
  TYPENAME_FLOAT,
  TYPENAME_DOUBLE,
  TYPENAME_LONG_DOUBLE,
  TYPENAME_POINTER_TO_CHAR,
  TYPENAME_POINTER_TO_VOID,
  TYPENAME_POINTER_TO_INT,
  TYPENAME_OTHER
};

#define typename(x) _Generic((x),					\
        _Bool: TYPENAME_BOOL,                            unsigned char: TYPENAME_UNSIGNED_CHAR, \
         char: TYPENAME_CHAR,                              signed char: TYPENAME_SIGNED_CHAR, \
    short int: TYPENAME_SHORT_INT,             unsigned short int: TYPENAME_UNSIGNED_SHORT_INT, \
          int: TYPENAME_INT,                               unsigned int: TYPENAME_UNSIGNED_INT, \
     long int: TYPENAME_LONG_INT,                unsigned long int: TYPENAME_UNSIGNED_LONG_INT, \
long long int: TYPENAME_LONG_LONG_INT, unsigned long long int: TYPENAME_UNSIGNED_LONG_LONG_INT, \
        float: TYPENAME_FLOAT,                                 double: TYPENAME_DOUBLE, \
  long double: TYPENAME_LONG_DOUBLE,                     char *: TYPENAME_POINTER_TO_CHAR, \
       void *: TYPENAME_POINTER_TO_VOID,                       int *: TYPENAME_POINTER_TO_CHAR, \
      default: TYPENAME_OTHER)

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
