#include "test.h"
#include "repl.h"

int main (int argc, char **argv) {
  if (argc == 2 && strcmp(argv[1], "-test") == 0) {
    run_ast_test();
    run_lexier_test();
    run_parser_test();
    run_eval_test();
    exit(0);
  }

  repl_start();
  
  return 0;
}
