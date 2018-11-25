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
  exit(1);
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
      exit(1);
    
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
      exit(1);
    
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
  printf("OK\n");
}
