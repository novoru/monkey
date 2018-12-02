#include "eval.h"
#include "lexier.h"
#include "object.h"
#include "parser.h"
#include "test.h"
#include "util.h"

Object *test_eval(char *input) {
  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Node *program = parse_program(p);

  return eval(program);
}

_Bool test_int_obj(Object *obj, long expected) {
  if (obj->ty != OBJ_INT) {
    DEBUG_PRINT("\n");
    printf("object is not int. got=%s\n", obj_type(obj->ty));
    return false;
  }

  if ((long)obj->value != expected) {
    printf("object has wrong value. got=%ld, want=%ld\n", (int)obj->value, expected);
    return false;
  }

  return true;
}

typedef struct int_test{
  char *input;
  long value;
} int_test;

int_test *new_int_test(char *input, long value) {
  int_test *t = malloc(sizeof(int_test));
  t->input = input;
  t->value = value;

  return t;
}

void test_eval_int_expr() {
  int_test *tests[] = { new_int_test("5", 5),
			new_int_test("10", 10)
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Object *evaluated = test_eval(tests[i]->input);
    test_int_obj(evaluated, tests[i]->value);
  }
}

void run_eval_test() {
  printf("=== test eval ===\n");
  printf("int\n");
  test_eval_int_expr();
  printf("OK\n");
}
