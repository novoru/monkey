#include "lexier.h"
#include "token.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#define PROMPT ">> "

void repl_start() {
  while (1) {
    char *input = readline(PROMPT);
    add_history(input);

    Lexier *l = new_lexier(input);
    Token *tok = next_token_lexier(l);

    while (tok->ty != TOK_EOF) {
      printf("{Type:%s Literal:%s}\n", token_type(tok->ty), tok->lit);
      tok = next_token_lexier(l);
    }

    free(input);
    del_lexier(l);
    del_token(tok);
  }
}
