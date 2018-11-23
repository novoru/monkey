#include "ast.h"
#include "lexier.h"
#include "parser.h"
#include "test.h"

void check_parser_error(Parser *p) {
  Vector *errors = p->errors;
  if (errors->len == 0)
    return;

  printf("parser has %d errors", errors->len);

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
      error("stmt0>token->lit not 'return'. got=%s\n", stmt->token->lit);
    
    del_node(stmt);
  }
  del_program(program);
  del_parser(p);
  del_lexier(l);
  
}

void run_parser_test() {
  printf("=== parser ===\n");
  printf("- let\n");
  test_let_stmts();
  printf("- return\n");
  test_return_stmts();
  printf("OK\n");
}
