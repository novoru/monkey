#include "ast.h"
#include "lexier.h"
#include "parser.h"
#include "test.h"

void check_parser_error(Parser *p) {
  Vector *errors = p->errors;
  if (errors->len == 0)
    return;

  printf("parser has %d errors\n", errors->len);

  for (int i = 0; i < errors->len; i++) {
    printf("%s\n", (char *)errors->data[i]);
  }
  exit(EXIT_FAILURE);
}

_Bool test_let_stmt(Node *s, char *name) {
  if (strcmp(s->token->lit, "let") != 0) {
    error("s->token->lit not 'let'. got=%s\n", s->token->lit);
    return 0;
  }

  if (s->ty != AST_LET) {
    error("s->token->ty not 'AST_LET'. got=%s\n",
	  node_type(s->token->ty));
    return 0;
  }

  if (strcmp(s->ident->name, name) != 0) {
    error("s->ident->name not '%s'. got=%s\n", name, s->ident->name);
    return 0;
  }

  return 1;
}

_Bool test_int_lit(Node *il, int value) {
  if (il->ty != AST_INT) {
    printf("il not AST_INT. got=%s\n", node_type(il->ty));
    return 0;
  }

  if (il->value != value) {
    printf("il->value not %d. got=%d\n", value, il->value);
    return 0;
  }

  if (strcmp(il->token->lit, format("%d", value)) != 0) {
    printf("il->token->lit not %d. got=%s\n", value, il->token->lit);
    return 0;
  }

  return 1;
  
}

_Bool test_ident(Node *ident, char *name){
  if (ident->ty != AST_IDENT) {
    printf("ident not AST_IDENT. got%s\n", node_type(ident->ty));
    return 0;
  }

  if (strcmp(ident->name, name) != 0) {
    printf("ident->name not %s. got=%s\n", name, ident->name);
    return 0;
  }

  if (strcmp(ident->token->lit, name) != 0) {
    printf("ident->token->lit not %s. got=%s\n", name, ident->token->lit);
    return 0;
  }

  return 1;
}

_Bool test_bool_lit(Node *exp, _Bool bool) {
  if (exp->ty != AST_BOOL) {
    printf("exp not AST_BOOL. got=%s\n", node_type(exp->ty));
    return 0;
  }

  if (exp->bool != bool) {
    printf("exp not %d. got=%d\n", bool, exp->bool);
    return 0;
  }

  if (strcmp(exp->token->lit, bool ? "true" : "false")) {
    printf("exp->token->lit not %s. got=%s\n",
	   bool ? "true" : "false",
	   exp->bool ? "true" : "false");
    return 0;
  }
  
  return 1;
}

_Bool test_lit_expr(Node *exp, void *expected, int ty) {
  switch (ty){
  case TYPENAME_INT:
    return test_int_lit(exp, (int)expected);
  case TYPENAME_POINTER_TO_CHAR:
    return test_ident(exp, (char *)expected);
  case TYPENAME_BOOL:
    return test_bool_lit(exp, (_Bool)expected);
  }
  
  printf("type of exp not handled. got=%d\n", typename(expected));
  return 0;
}

_Bool test_inf_expr(Node *exp,
		    void *left,  int ty_left,
		    char *op,
		    void *right, int ty_right) {
  if (exp->ty != AST_INF_EXPR) {
    printf("exp is not AST_INF_EXPR. got=%s\n", node_type(exp->ty));
    return 0;
  }

  if (!test_lit_expr(exp->left, left, ty_left))
    return 0;

  if (strcmp(exp->op, op) != 0) {
    printf("exp->op is not '%s'. got=%s\n", op, exp->op);
    return 0;
  }

  if (!test_lit_expr(exp->right, right, ty_right))
    return 0;

  return 1;
}

void test_let_stmts() {
  char *input = "   \
let x = 5;	    \
let y = 10;	    \
let foobar = 838383; \
";

  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);

  if (program == NULL)
    error("parse_program(p) returned NULL\n");

  if (program->stmts->len < 3)
    error("program->stmts does not contain 3 statements.got=%d\n",
	  program->stmts->len);

  char *expects[] = {"x", "y", "foobar"};

  for (int i = 0; i < LENGTH(expects); i++) {
    Node *stmt = (Node *)program->stmts->data[i];
    if (!test_let_stmt(stmt, expects[i]))
      return;
    del_node(stmt);
  }
  del_program(program);
  del_parser(p);
  del_lexier(l);
  
}

void test_return_stmts() {
  char *input = "   \
return 5;	   \
return 10;	   \
return 993322;	   \
";

  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);

  check_parser_error(p);

  if (program == NULL)
    error("parse_program(p) returned NULL\n");

  
  for (int i = 0; i < program->stmts->len; i++) {
    Node *stmt = (Node *)program->stmts->data[i];

    if (stmt->ty != AST_RETURN)
      error("stmt not AST_RETURN. got=%s\n", node_type(stmt->ty));

    if (strcmp(stmt->token->lit, "return") != 0)
      error("stmt->token->lit not 'return'. got=%s\n", stmt->token->lit);
    
    del_node(stmt);
  }
  del_program(program);
  del_parser(p);
  del_lexier(l);
  
}

void test_ident_expr() {
  char *input = "foobar;";

  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);

  check_parser_error(p);

  if (program == NULL)
    error("parse_program(p) returned NULL\n");


  if (program->stmts->len < 1)
    error("program->stmts does not contain 1 statements.got=%d\n",
	  program->stmts->len);

  Node *exp   = (Node *)program->stmts->data[0];
  Node *ident = exp->expr;
  
  if (ident->ty != AST_IDENT)
    error("ident not AST_IDENT. got=%s\n", node_type(ident->ty));
  
  if (strcmp(ident->name, "foobar") != 0)
    error("ident->name not 'foobar'. got=%s\n", ident->name);
    
  if (strcmp(ident->token->lit, "foobar") != 0)
    error("ident->token->lit not 'foobar'. got=%s\n", ident->token->lit);
  
  del_node(ident);
  del_program(program);
  del_parser(p);
  del_lexier(l);

}

void test_int_expr() {
  
  char *input = "5;";
  
  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);
  
  check_parser_error(p);

  if (program == NULL)
    error("parse_program(p) returned NULL\n");


  if (program->stmts->len < 1)
    error("program->stmts does not contain 1 statements.got=%d\n",
	  program->stmts->len);
  
  Node *int_lit = (Node *)program->stmts->data[0];
    
  if (int_lit->expr->ty != AST_INT)
    error("int_lit not AST_INT. got=%s\n", node_type(int_lit->expr->ty));
  
  if (int_lit->expr->value != 5)
    error("int_lit->expr-v>value not '5'. got=%s\n", int_lit->expr->value);
    
  if (strcmp(int_lit->expr->token->lit, "5") != 0)
    error("int_lit->expr->token->lit not '5'. got=%s\n", int_lit->expr->token->lit);
 
  del_node(int_lit);
  del_program(program);
  del_parser(p);
  del_lexier(l);
}

typedef struct pref_test {
  char *input;
  char *op;
  int value;
} pref_test;

static pref_test *new_pref_test(char *input, char *op, int value) {
  pref_test *t = malloc(sizeof(pref_test));
  t->input = input;
  t->op = op;
  t->value = value;

  return t;
}

void test_pref_expr() {
  pref_test *pref_tests[] = { new_pref_test("!5;", "!", 5),
			      new_pref_test("-15;", "-",15)
  };

  for (int i = 0; i < LENGTH(pref_tests); i++) {
    pref_test *t = pref_tests[i];
    Lexier *l = new_lexier(t->input);
    Parser *p = new_parser(l);
    Program *program = parse_program(p);

    check_parser_error(p);

    if (program == NULL)
      error("parse_program(p) returned NULL\n");
    
    
    if (program->stmts->len < 1)
      error("program->stmts does not contain 1 statements.got=%d\n",
	    program->stmts->len);
    
    Node *exp = (Node *)program->stmts->data[0];
    Node *expr = exp->expr;
    
    if (expr->ty != AST_PREF_EXPR)
      error("expr->ty not AST_PREF_EXPR. got=%s\n", node_type(expr->ty));
    
    if (strcmp(expr->op, t->op) != 0)
      error("expr->op not '%s'. got=%s\n", t->op, expr->op);

    if (!test_int_lit(expr->right, t->value))
      exit(EXIT_FAILURE);
    
    del_node(expr);
    del_program(program);
    del_parser(p);
    del_lexier(l);
  
  }
  
}

typedef struct inf_test {
  char *input;
  void *left;
  int ty_left;
  char *op;
  void *right;
  int ty_right;
} inf_test;

inf_test *new_inf_test(char *input,
		       void *left, int ty_left,
		       char *op,
		       void *right, int ty_right) {
  inf_test *t = malloc(sizeof(inf_test));
  t->input = input;
  t->left = left;
  t->ty_left = ty_left;
  t->op = op;
  t->right = right;
  t->ty_right = ty_right;
  
  return t;
}

void test_parse_inf_exprs() {
  inf_test *inf_tests[] = { new_inf_test("5 + 5;",  (void *)5, TYPENAME_INT, "+", (void *)5, TYPENAME_INT),
			    new_inf_test("5 - 5;",  (void *)5, TYPENAME_INT, "-", (void *)5, TYPENAME_INT),
			    new_inf_test("5 * 5;",  (void *)5, TYPENAME_INT, "*", (void *)5, TYPENAME_INT),
			    new_inf_test("5 / 5;",  (void *)5, TYPENAME_INT, "/", (void *)5, TYPENAME_INT),
			    new_inf_test("5 > 5;",  (void *)5, TYPENAME_INT, ">", (void *)5, TYPENAME_INT),
			    new_inf_test("5 < 5;",  (void *)5, TYPENAME_INT, "<", (void *)5, TYPENAME_INT),
			    new_inf_test("5 == 5;", (void *)5, TYPENAME_INT, "==", (void *)5, TYPENAME_INT),
			    new_inf_test("5 != 5;", (void *)5, TYPENAME_INT, "!=", (void *)5, TYPENAME_INT),
			    new_inf_test("true == true",   (void *)true, TYPENAME_BOOL,  "==", (void *)true, TYPENAME_BOOL),
			    new_inf_test("true != false",  (void *)true, TYPENAME_BOOL,  "!=", (void *)false, TYPENAME_BOOL),
			    new_inf_test("false == false", (void *)false, TYPENAME_BOOL, "==", (void *)false, TYPENAME_BOOL),
  };

  for (int i = 0; i < LENGTH(inf_tests); i++) {
    inf_test *t = inf_tests[i];
    Lexier *l = new_lexier(t->input);
    Parser *p = new_parser(l);
    Program *program = parse_program(p);

    check_parser_error(p);

    if (program == NULL)
      error("parse_program(p) returned NULL\n");
    
    
    if (program->stmts->len < 1)
      error("program->stmts does not contain 1 statements.got=%d\n",
	    program->stmts->len);
    
    Node *exp = (Node *)program->stmts->data[0];
    Node *expr = exp->expr;
    
    if (expr->ty != AST_INF_EXPR)
      error("expr->ty not AST_INF_EXPR. got=%s\n", node_type(expr->ty));

    if (!test_inf_expr(expr,
		       t->left,  t->ty_left,
		       t->op,
		       t->right, t->ty_right))
      exit(EXIT_FAILURE);
    
    del_node(expr);
    del_program(program);
    del_parser(p);
    del_lexier(l);
  
  }  
  
}

typedef struct bool_test {
  char *input;
  _Bool expected;
} bool_test;

bool_test *new_bool_test(char *input, _Bool expected) {
  bool_test *t = malloc(sizeof(bool_test));
  t->input = input;
  t->expected = expected;

  return t;
}

void test_bool_expr() {
  bool_test *bool_tests[] = { new_bool_test("true", true),
			      new_bool_test("false", false)
  };

  for (int i = 0; i < LENGTH(bool_tests); i++) {
    bool_test *t = bool_tests[i];
    Lexier *l = new_lexier(t->input);
    Parser *p = new_parser(l);
    Program *program = parse_program(p);

    check_parser_error(p);

    if (program == NULL)
      error("parse_program(p) returned NULL\n");
    
    if (program->stmts->len < 1)
      error("program->stmts does not contain 1 statements.got=%d\n",
	    program->stmts->len);
    
    Node *exp = (Node *)program->stmts->data[0];
    Node *expr = exp->expr;
    
    if (expr->ty != AST_BOOL)
      error("expr->ty not AST_BOOL. got=%s\n", node_type(expr->ty));

    if (expr->bool != t->expected)
      error("expr->bool not '%s'. got=%s\n",
	    t->expected ? "true" : "false",
	    expr->bool  ? "true" : "false");
    
    del_node(expr);
    del_program(program);
    del_parser(p);
    del_lexier(l);
  
  }  
  
}

typedef struct op_prec_test {
  char *input;
  char *expected;
} op_prec_test;

op_prec_test *new_op_prec_test(char *input, char *expected) {
  op_prec_test *t = malloc(sizeof(op_prec_test));
  t->input = input;
  t->expected = expected;

  return t;
}

void test_op_prec_parsing() {
  op_prec_test *op_prec_tests[] = { new_op_prec_test("-a * b;", "((-a) * b)"),
				    new_op_prec_test("!-a;", "(!(-a))"),
				    new_op_prec_test("a + b + c;", "((a + b) + c)"),
				    new_op_prec_test("a + b - c;", "((a + b) - c)"),
				    new_op_prec_test("a * b * c;", "((a * b) * c)"),
				    new_op_prec_test("a * b / c;", "((a * b) / c)"),
				    new_op_prec_test("a + b / c;", "(a + (b / c))"),
				    new_op_prec_test("a + b * c + d / e - f;",
						     "(((a + (b * c)) + (d / e)) - f)"),
				    new_op_prec_test("3 + 4; -5 * 5;", "(3 + 4)((-5) * 5)"),
				    new_op_prec_test("5 > 4 == 3 < 4;", "((5 > 4) == (3 < 4))"),
				    new_op_prec_test("5 < 4 != 3 > 4;", "((5 < 4) != (3 > 4))"),
				    new_op_prec_test("3 + 4 * 5 == 3 * 1 + 4 * 5;",
						     "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"),
				    new_op_prec_test("true;", "true"),
				    new_op_prec_test("true;", "true"),
				    new_op_prec_test("false;", "false"),
				    new_op_prec_test("3 > 5 == false;", "((3 > 5) == false)"),
				    new_op_prec_test("3 < 5 == true;", "((3 < 5) == true)"),
				    new_op_prec_test("1 + (2 + 3) + 4;", "((1 + (2 + 3)) + 4)"),
				    new_op_prec_test("(5 + 5) * 2;", "((5 + 5) * 2)"),
				    new_op_prec_test("2 / (5 + 5);", "(2 / (5 + 5))"),
				    new_op_prec_test("-(5 + 5);", "(-(5 + 5))"),
				    new_op_prec_test("!(true == true);", "(!(true == true))"),
  };

    for (int i = 0; i < LENGTH(op_prec_tests); i++) {
      op_prec_test *t = op_prec_tests[i];

      Lexier *l = new_lexier(t->input);
      Parser *p = new_parser(l);
      Program *program = parse_program(p);
        
      check_parser_error(p);

      char *actual = program_to_str(program);
      
      if (strcmp(actual, t->expected) != 0)
	error("expected=%s, got=%s\n", t->expected, actual);

      del_program(program);
      del_parser(p);
      del_lexier(l);
  
  }  

}

void test_if_expr() {
  char *input = "if (x < y) { x }";
  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);

  check_parser_error(p);

  if (program == NULL)
    error("parse_program(p) returned NULL\n");
    
  if (program->stmts->len != 1)
    error("program->stmts does not contain 1 statements.got=%d\n",
	  program->stmts->len);
    
  Node *stmt = (Node *)program->stmts->data[0];
  Node *expr = stmt->expr;
    
  if (expr->ty != AST_IF_EXPR)
    error("expr->ty not AST_IF_EXPR. got=%s\n", node_type(expr->ty));

  if ( test_inf_expr(expr->cond,
		     "x", TYPENAME_POINTER_TO_CHAR,
		     "<",
		     "y", TYPENAME_POINTER_TO_CHAR)
       )
    return;

  if (expr->conseq->stmts->len != 1)
    error("consequences is not 1 statements. got=%d\n", expr->conseq->stmts->len);

  Node *conseq = (Node *)expr->conseq->stmts->data[0];

  if (conseq->ty != AST_EXPR_STMT)
    error("expe->conseq->stmts->data[0] is not AST_EXPR_STMT. got=%s\n", node_type(conseq->ty));

  if (test_ident(conseq->expr, "x"))
    return;

  if (expr->alter != NULL)
    error("expr->alter was not NULL.\n");
  
  del_node(expr);
  del_program(program);
  del_parser(p);
  del_lexier(l);

}

void test_if_else_expr() {
  char *input = "if (x < y) { x } else { y }";
  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);

  check_parser_error(p);

  if (program == NULL)
    error("parse_program(p) returned NULL\n");
    
  if (program->stmts->len != 1)
    error("program->stmts does not contain 1 statements.got=%d\n",
	  program->stmts->len);
    
  Node *stmt = (Node *)program->stmts->data[0];
  Node *expr = stmt->expr;
    
  if (expr->ty != AST_IF_EXPR)
    error("expr->ty not AST_IF_EXPR. got=%s\n", node_type(expr->ty));

  if ( test_inf_expr(expr->cond,
		     "x", TYPENAME_POINTER_TO_CHAR,
		     "<",
		     "y", TYPENAME_POINTER_TO_CHAR)
       )
    return;

  if (expr->conseq->stmts->len != 1)
    error("consequences is not 1 statements. got=%d\n", expr->conseq->stmts->len);

  Node *conseq = (Node *)expr->conseq->stmts->data[0];

  if (conseq->ty != AST_EXPR_STMT)
    error("expr->conseq->stmts->data[0] is not AST_EXPR_STMT. got=%s\n", node_type(conseq->ty));

  if (test_ident(conseq->expr, "x"))
    return;

  Node *alter = (Node *)expr->alter->stmts->data[0];

  if (alter->ty != AST_EXPR_STMT)
    error("expr->alter->stmts->data[0] is not AST_EXPR_STMT. got=%s\n", node_type(alter->ty));
  
  if (test_ident(alter->expr, "y"))
    return;

  del_node(expr);
  del_program(program);
  del_parser(p);
  del_lexier(l);

}

void test_func_lit_parsing() {

  char *input = "fn(x, y) { x + y;}";
  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);

  check_parser_error(p);

  if (program == NULL)
    error("parse_program(p) returned NULL\n");
    
  if (program->stmts->len != 1)
    error("program->stmts does not contain 1 statements.got=%d\n",
	  program->stmts->len);

  Node *stmt = (Node *)program->stmts->data[0];
  
  if (stmt->ty != AST_EXPR_STMT)
    error("program->stmts->data[0] is not AST_EXPR_STMT. got=%s\n", node_type(stmt->ty));

  Node *func = (Node *)stmt->expr;

  if (func->ty != AST_FUNCTION)
    error("stmt->expr is not AST_FUNCTION. got=%s\n", node_type(func->ty));

  if (func->params->len != 2)
    error("function literal parameters wrong. want 2, got=%d\n", func->params->len);

  test_lit_expr((Node *)func->params->data[0], "x", TYPENAME_POINTER_TO_CHAR);
  test_lit_expr((Node *)func->params->data[1], "y", TYPENAME_POINTER_TO_CHAR);

  if (func->body->stmts->len != 1)
    error("func->body->stmts->len has not 1 statements. got=%d\n", func->body->stmts->len);

  Node *body_stmt = (Node *)func->body->stmts->data[0];

  if (body_stmt->ty != AST_EXPR_STMT)
    error("function body stmt is not AST_EXPR_STMT. got=%s\n", node_type(body_stmt->ty));

  test_inf_expr(body_stmt->expr,
		"x", TYPENAME_POINTER_TO_CHAR,
		"+",
		"y", TYPENAME_POINTER_TO_CHAR);

}

typedef struct func_param_test {
  char *input;
  Vector *expected;
} func_param_test;

func_param_test *new_func_param_test(char *input, char *expected[], int len) {
  func_param_test *t = malloc(sizeof(func_param_test));
  t->input = input;
  t->expected = new_vector();

  int count = 0;
  for (int i = 0; i < len; i++){
    if (expected[i] == NULL) return t;
    vec_push(t->expected, (void *)expected[i]);
    count++;
  }
  
  return t;
}

void test_func_param_parsing() {
  char *ex1[1] = {};
  char *ex2[1] = {"x"};
  char *ex3[3] = {"x", "y", "z"};
  
  func_param_test *tests[] = { new_func_param_test("fn() {};",        ex1, LENGTH(ex1)),
			       new_func_param_test("fn(x) {};",       ex2, LENGTH(ex2)),
			       new_func_param_test("fn(x, y, z) {};", ex3, LENGTH(ex3))
  };

  for (int i = 0; i < LENGTH(tests); i++) {
    Lexier *l = new_lexier(tests[i]->input);
    Parser *p = new_parser(l);
    Program *program = parse_program(p);

    check_parser_error(p);
    
    Node *stmt = (Node *)program->stmts->data[0];
    Node *func = stmt->expr;
    Vector *expected = tests[i]->expected;
    if (func->params->len != expected->len)
      error("length parameters wrong. want %d, got=%d\n", expected->len, func->params->len);

    for (int j = 0; j < func->params->len; j++) {
      test_lit_expr(func->params->data[j], (void *)expected->data[j], TYPENAME_POINTER_TO_CHAR);
    }
    
  }
  
}

void test_call_expr() {
  char *input = "add(1, 2 * 3, 4 + 5)";

  Lexier *l = new_lexier(input);
  Parser *p = new_parser(l);
  Program *program = parse_program(p);

  check_parser_error(p);

  if (program->stmts->len != 1)
    error("program->stmts does not contain 1 statements.got=%d\n",
	  program->stmts->len);

  Node *stmt = (Node *)program->stmts->data[0];
  
  if (stmt->ty != AST_EXPR_STMT)
    error("program->stmts->data[0] is not AST_CALL_EXPR_STMT. got=%s\n", node_type(stmt->ty));

  Node *expr = stmt->expr;

  if (expr->ty != AST_CALL_EXPR)
    error("stmt->expr is not AST_CALL_EXPR. got=%s\n", node_type(expr->ty));

  if (!test_ident(expr->func, "add"))
    return;

  if (expr->args->len != 3)
    error("wrong length of arguments. got=%d\n", expr->args->len);

  test_lit_expr(expr->args->data[0], (void *)1, TYPENAME_INT);
  test_inf_expr(expr->args->data[1], (void *)2, TYPENAME_INT, "*", 3, TYPENAME_INT);
  test_inf_expr(expr->args->data[2], (void *)4, TYPENAME_INT,  "+", 5, TYPENAME_INT);
  
}

void run_parser_test() {
  printf("=== test parser ===\n");
  printf("- let\n");
  test_let_stmts();
  printf("- return\n");
  test_return_stmts();
  printf("- ident\n");
  test_ident_expr();
  printf("- int\n");
  test_int_expr();
  printf("- prefix\n");
  test_pref_expr();
  printf("- infix\n");
  test_parse_inf_exprs();
  printf("- bool\n");
  test_bool_expr();
  printf("- operator precedence\n");
  test_op_prec_parsing();
  printf("- if\n");
  test_if_expr();
  printf("- if else\n");
  test_if_else_expr();
  printf("- function\n");
  test_func_lit_parsing();
  printf("- function parameters\n");
  test_func_param_parsing();
  printf("- call expression\n");
  test_call_expr();
  printf("OK\n");
}
