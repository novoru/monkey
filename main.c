#include "test.h"

int main (int argc, char **argv) {
  if (argc == 2 && strcmp(argv[1], "-test") == 0) {
    run_lexier_test();
  }

  
  return 0;
}
