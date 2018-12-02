#include "ast.h"
#include "test.h"
#include "token.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

void test_string() {
  Token *let_tok = new_token(TOK_LET, "let");
  Token *ident_tok = new_token(TOK_IDENT, "myVar");
  Token *data_tok = new_token(TOK_IDENT, "anotherVar");
  Node *ident = new_ident(ident_tok);
  Node *data = new_ident(data_tok);
  Node *let_stmt = new_let_stmt(let_tok);

  let_stmt->ident = ident;
  let_stmt->data = data;

  Node *program = new_program();

  vec_push(program->stmts, (void *)let_stmt);

  if (strcmp(program_to_str(program), "let myVar = anotherVar;") != 0)
    error("program_to_str(program) wrong. got=%s\n", program_to_str(program));

}

void run_ast_test() {
  printf("=== ast ===\n");
  test_string();
  printf("OK\n");
}
