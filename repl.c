#include "ast.h"
#include "lexier.h"
#include "parser.h"
#include "token.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#define PROMPT ">> "

void print_parse_errors(Vector *errors) {
  for (int i = 0; i < errors->len; i++)
    printf("\t%s\t\n", (char *)errors->data[i]);
}

void repl_start() {
  while (1) {
    char *input = readline(PROMPT);
    add_history(input);

    Lexier *l = new_lexier(input);
    Parser *p = new_parser(l);

    Node *program = parse_program(p);

    if (p->errors->len != 0) {
      print_parse_errors(p->errors);
      continue;
    }

    char *str = program_to_str(program);
    
    if (str != NULL)
      printf("%s\n", program_to_str(program));
  }
}
