#include "ast.h"
#include "test.h"
#include "token.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

void test_string() {
  Token *let_tok = new_token(TOK_LET, "let");
  Token *name = new_ident("myVar");
  Token *value = new_ident("anotherVar");
  Node *let_stmt = new_let_stmt(let_tok);
  let_stmt->name = name;
  let_stmt->value = value;

  Program *program = new_program();
  vec_push(program->stmts, let_stmt);
  
  if (strcmp(program_to_str(program), "let myVar = anotherVar;") != 0)
    error("program_to_str(program) wrong. got=%s\n", program_to_str(program));
  
}

void run_ast_test() {
  printf("=== ast ===\n");
  test_string();
  printf("=== OK ===\n");
}
