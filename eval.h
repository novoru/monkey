#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "object.h"
#include "util.h"

#include <stdarg.h>
#include <string.h>

Object *eval(Node *node);

#endif
