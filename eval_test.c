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
    printf("object is not int. got=%s\n", obj_type(obj->ty));
    return false;
  }

  if ((long)obj->value != expected) {
    printf("object has wrong value. got=%ld, want=%ld\n", (int)obj->value, expected);
    return false;
  }

  return true;
}

_Bool test_bool_obj(Object *obj, _Bool expected) {
  if (obj->ty != OBJ_BOOL) {
    printf("object is not bool. got=%s\n", obj_type(obj->ty));
    return false;
  }

  if (!((_Bool)obj->value == expected)) {
    printf("object has wrong value. got=%s, want=%s\n",
	   inspect_obj(obj),expected ? "true" : "false" );
    return false;
  }

  return true;
}

typedef struct int_obj_test{
  char *input;
  long expected;
} int_obj_test;

int_obj_test *new_int_obj_test(char *input, long expected) {
  int_obj_test *t = malloc(sizeof(int_obj_test));
  t->input = input;
  t->expected = expected;

  return t;
}

void test_eval_int_expr() {
  int_obj_test *tests[] = { new_int_obj_test("5", 5),
			    new_int_obj_test("10", 10)
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Object *evaluated = test_eval(tests[i]->input);
    test_int_obj(evaluated, tests[i]->expected);
  }
}

typedef struct bool_obj_test{
  char *input;
  _Bool expected;
} bool_obj_test;

bool_obj_test *new_bool_obj_test(char *input, _Bool expected) {
  bool_obj_test *t = malloc(sizeof(bool_obj_test));
  t->input = input;
  t->expected = expected;

  return t;
}

void test_eval_bool_expr() {
  bool_obj_test *tests[] = { new_bool_obj_test("true",  true),
			     new_bool_obj_test("false", false)
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Object *evaluated = test_eval(tests[i]->input);
    test_bool_obj(evaluated, tests[i]->expected);
  }
}

void test_bang_op() {
  bool_obj_test *tests[] = { new_bool_obj_test("!true", false),
			     new_bool_obj_test("!false", true),
			     new_bool_obj_test("!5", false),
			     new_bool_obj_test("!!true", true),
			     new_bool_obj_test("!!false", false),
			     new_bool_obj_test("!!5", true)
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Object *evaluated = test_eval(tests[i]->input);
    test_bool_obj(evaluated, tests[i]->expected);
  }
}

void run_eval_test() {
  printf("=== test eval ===\n");
  printf("- int\n");
  test_eval_int_expr();
  printf("- bool\n");
  test_eval_bool_expr();
  printf("- bang\n");
  test_bang_op();
  printf("OK\n");
}

