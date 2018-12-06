#include "eval.h"
#include "lexier.h"
#include "object.h"
#include "parser.h"
#include "test.h"
#include "util.h"

static Object *test_eval(char *input) {
  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Node *program = parse_program(p);

  return eval(program);
}

static _Bool test_int_obj(Object *obj, long expected) {
  if (obj->ty != OBJ_INT) {
    printf("object is not int. got=%s\n", obj_type(obj));
    return false;
  }

  if ((long)obj->value != expected) {
    printf("object has wrong value. got=%ld, want=%ld\n", (int)obj->value, expected);
    return false;
  }

  return true;
}

static _Bool test_bool_obj(Object *obj, _Bool expected) {
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

static _Bool test_null_obj(Object *obj) {
  if (obj->ty != OBJ_NULL) {
    printf("object is not NULL. got=%s\n", obj_type(obj));
    return false;
  }

  return true;
}

typedef struct int_obj_test{
  char *input;
  long expected;
} int_obj_test;

static int_obj_test *new_int_obj_test(char *input, long expected) {
  int_obj_test *t = malloc(sizeof(int_obj_test));
  t->input = input;
  t->expected = expected;

  return t;
}

static void test_eval_int_expr() {
  int_obj_test *tests[] = { new_int_obj_test("5", 5),
			    new_int_obj_test("10", 10),
			    new_int_obj_test("-5", -5),
			    new_int_obj_test("-10", -10),
			    new_int_obj_test("5 + 5 + 5 + 5 - 10", 10),
			    new_int_obj_test("2 * 2 * 2 * 2 * 2", 32),
			    new_int_obj_test("-50 + 100 + -50", 0),
			    new_int_obj_test("5 * 2 + 10", 20),
			    new_int_obj_test("5 + 2 * 10", 25),
			    new_int_obj_test("20 + 2 * -10", 0),
			    new_int_obj_test("50 / 2 * 2 + 10", 60),
			    new_int_obj_test("2 * (5 + 10)", 30),
			    new_int_obj_test("3 * 3 * 3 +10", 37),
			    new_int_obj_test("3 * (3 * 3) + 10", 37),
			    new_int_obj_test("(5 + 10 * 2 + 15 / 3) * 2 + -10", 50)
			    
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

static bool_obj_test *new_bool_obj_test(char *input, _Bool expected) {
  bool_obj_test *t = malloc(sizeof(bool_obj_test));
  t->input = input;
  t->expected = expected;

  return t;
}

static void test_eval_bool_expr() {
  bool_obj_test *tests[] = { new_bool_obj_test("true",  true),
			     new_bool_obj_test("false", false),
			     new_bool_obj_test("1 < 2", true),
			     new_bool_obj_test("1 > 2", false),
			     new_bool_obj_test("1 < 1", false),
			     new_bool_obj_test("1 > 1", false),
			     new_bool_obj_test("1 == 1", true),
			     new_bool_obj_test("1 != 1", false),
			     new_bool_obj_test("1 == 2", false),
			     new_bool_obj_test("1 != 2", true),
			     new_bool_obj_test("true == true", true),
			     new_bool_obj_test("false == false", true),
			     new_bool_obj_test("true == false", false),
			     new_bool_obj_test("true != false", true),
			     new_bool_obj_test("false != true", true),
			     new_bool_obj_test("(1 < 2) == true", true),
			     new_bool_obj_test("(1 < 2) == false", false),
			     new_bool_obj_test("(1 > 2) == true", false),
			     new_bool_obj_test("(1 > 2) == false", true),
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Object *evaluated = test_eval(tests[i]->input);
    test_bool_obj(evaluated, tests[i]->expected);
  }
}

static void test_bang_op() {
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

typedef struct if_else_test{
  char *input;
  void *expected;
  int ty;
} if_else_test;

static if_else_test *new_if_else_test(char *input, void *expected, int ty) {
  if_else_test *t = malloc(sizeof(if_else_test));
  t->input = input;
  t->expected = expected;
  t->ty = ty;

  return t;
}

static void test_if_else_expr() {
  if_else_test *tests[] = { new_if_else_test("if (true) { 10 }", (void *)10, TYPENAME_INT),
			    new_if_else_test("if (false) { 10 }", NULL, TYPENAME_OTHER),
			    new_if_else_test("if (1) { 10 }", (void *)10, TYPENAME_INT),
			    new_if_else_test("if (1 < 2) { 10 }", (void *)10, TYPENAME_INT),
			    new_if_else_test("if (1 > 2) { 10 }", NULL, TYPENAME_OTHER),
			    new_if_else_test("if (1 > 2) { 10 } else {20}", (void *)20, TYPENAME_INT),
			    new_if_else_test("if (1 < 2) { 10 } else {20}", (void *)10, TYPENAME_INT)
			    
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Object *evaluated = test_eval(tests[i]->input);
    if (tests[i]->ty == TYPENAME_INT)
      test_int_obj(evaluated, (long)tests[i]->expected);
    else
      test_null_obj(evaluated);
  }
}

static void test_return_stmts() {
  typedef struct test{
    char *input;
    long expected;
  } test;
  
  test tests[] = { {"return 10;", 10},
		   {"return 10; 9;", 10},
		   {"return 2 * 5; 9;", 10},
		   {"9; return 2 * 5; 9;", 10},
		   {"if (10 > 1) { if (10 > 1) { return 10;} return 1;}", 10}
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Object *evaluated = test_eval(tests[i].input);
    test_int_obj(evaluated, tests[i].expected);
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
  printf("- if else\n");
  test_if_else_expr();
  printf("- return\n");
  test_return_stmts();
  printf("OK\n");
}

