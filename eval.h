#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "env.h"
#include "object.h"
#include "util.h"

#include <stdarg.h>
#include <string.h>

Object *get_null_obj();
Object *eval(Node *node, Env *env);

#endif
