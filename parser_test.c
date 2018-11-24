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

  Node *ident = (Node *)program->stmts->data[0];
    
  if (ident->expr->ty != AST_IDENT)
    error("ident not AST_IDENT. got=%s\n", node_type(ident->expr->ty));
  
  if (strcmp(ident->expr->name, "foobar") != 0)
    error("ident->expr->name not 'foobar'. got=%s\n", ident->expr->name);
    
  if (strcmp(ident->expr->token->lit, "foobar") != 0)
    error("ident->expr->token->lit not 'foobar'. got=%s\n", ident->expr->token->lit);
  
  //del_node(ident->expr);
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
  printf("OK\n");
}
