#include "lexier.h"
#include "token.h"
#include "util.h"
#include "test.h"

#include <string.h>


typedef struct test_token {
  int expected_ty;
  char *expected_lit;
} test_token;

test_token *new_test_token(int ty, char *lit) {
  test_token *tt = malloc(sizeof(test_token));
  tt->expected_ty = ty;
  tt->expected_lit = lit;

  return tt;
}

void del_test_token(test_token *tt) {
  free(tt);
}

void test_next_token() {
  char *input = "let five = 5;  \
let ten = 10;			\
				\
let add = fn(x, y) {	      	\
  x + y;			\
};				\
				\
let result = add(five, ten);	\
!-/*5;				\
5 < 10 > 5;			\
if (5 < 10) {			\
  return true;			\
} else {			\
  return false;			\
}				\
				\
10 == 10;			\
10 != 9;			\
";
  
  test_token *tests[] = { new_test_token(TOK_LET,       "let"),
			  new_test_token(TOK_IDENT,     "five"),
			  new_test_token(TOK_ASSIGN,    "="),
			  new_test_token(TOK_INT,       "5"),
			  new_test_token(TOK_SEMICOLON, ";"),
			  
			  new_test_token(TOK_LET,       "let"),
			  new_test_token(TOK_IDENT,     "ten"),
			  new_test_token(TOK_ASSIGN,    "="),
			  new_test_token(TOK_INT,       "10"),
			  new_test_token(TOK_SEMICOLON, ";"),
			  
			  new_test_token(TOK_LET,       "let"),
			  new_test_token(TOK_IDENT,     "add"),
			  new_test_token(TOK_ASSIGN,    "="),
			  new_test_token(TOK_FUNCTION,  "fn"),
			  new_test_token(TOK_LPAREN,    "("),
			  new_test_token(TOK_IDENT,     "x"),
			  new_test_token(TOK_COMMA,     ","),
			  new_test_token(TOK_IDENT,     "y"),
			  new_test_token(TOK_RPAREN,    ")"),
			  new_test_token(TOK_LBRACE,    "{"),
			  new_test_token(TOK_IDENT,     "x"), 
			  new_test_token(TOK_PLUS,      "+"),
			  new_test_token(TOK_IDENT,     "y"),
			  new_test_token(TOK_SEMICOLON, ";"),
			  
			  new_test_token(TOK_RBRACE,    "}"),
			  new_test_token(TOK_SEMICOLON, ";"),
			  
			  new_test_token(TOK_LET,       "let"),
			  new_test_token(TOK_IDENT,     "result"),
			  new_test_token(TOK_ASSIGN,    "="),
			  new_test_token(TOK_IDENT,     "add"),
  			  new_test_token(TOK_LPAREN,    "("),
  			  new_test_token(TOK_IDENT,     "five"),
  			  new_test_token(TOK_COMMA,     ","),
  			  new_test_token(TOK_IDENT,     "ten"),
  			  new_test_token(TOK_RPAREN,    ")"),
  			  new_test_token(TOK_SEMICOLON, ";"),
			  
  			  new_test_token(TOK_BANG,      "!"),
  			  new_test_token(TOK_MINUS,     "-"),
  			  new_test_token(TOK_SLASH,     "/"),
  			  new_test_token(TOK_ASTERISK,  "*"),
  			  new_test_token(TOK_INT,       "5"),
  			  new_test_token(TOK_SEMICOLON, ";"),
			  
  			  new_test_token(TOK_INT,       "5"),
  			  new_test_token(TOK_LT,        "<"),
  			  new_test_token(TOK_INT,       "10"),
  			  new_test_token(TOK_GT,        ">"),
  			  new_test_token(TOK_INT,       "5"),
  			  new_test_token(TOK_SEMICOLON, ";"),
			  
			  new_test_token(TOK_IF,        "if"),
			  new_test_token(TOK_LPAREN,    "("),
			  new_test_token(TOK_INT,       "5"),
			  new_test_token(TOK_LT,        "<"),
			  new_test_token(TOK_INT,       "10"),
			  new_test_token(TOK_RPAREN,    ")"),
			  new_test_token(TOK_LBRACE,    "{"),
  			  
			  new_test_token(TOK_RETURN,    "return"),
			  new_test_token(TOK_TRUE,      "true"),
			  new_test_token(TOK_SEMICOLON, ";"),

			  new_test_token(TOK_RBRACE,    "}"),
			  new_test_token(TOK_ELSE,      "else"),
			  new_test_token(TOK_LBRACE,    "{"),

			  new_test_token(TOK_RETURN,    "return"),
			  new_test_token(TOK_FALSE,     "false"),
			  new_test_token(TOK_SEMICOLON, ";"),

			  new_test_token(TOK_RBRACE,    "}"),

			  new_test_token(TOK_INT,       "10"),
			  new_test_token(TOK_EQ,        "=="),
			  new_test_token(TOK_INT,       "10"),
			  new_test_token(TOK_SEMICOLON, ";"),
			  
			  new_test_token(TOK_INT,       "10"),
			  new_test_token(TOK_NOT_EQ,    "!="),
			  new_test_token(TOK_INT,       "9"),
			  new_test_token(TOK_SEMICOLON, ";"),

  			  new_test_token(TOK_EOF,       "")
                     };
  
  Lexier *l = new_lexier(input);

  for (int i = 0; i < LENGTH(tests); i++) {
    Token *tok = next_token_lexier(l);

    DEBUG_PRINT("tok->lit: %s\n", tok->lit);
    
    if (tok->ty != tests[i]->expected_ty) {
      DEBUG_PRINT("tok->lit: %s\n", tok->lit);
      error("tests[%d] - tokentype wrong. expected=%d, got=%d",
	    i, tests[i]->expected_ty, tok->ty);
    }
    
    if (strcmp(tok->lit, tests[i]->expected_lit) != 0) {
      DEBUG_PRINT("tok->lit: %s\n", tok->lit);
      error("tests[%d] = literal wrong. expected=%s, got=%s",
	    i, tests[i]->expected_lit, tok->lit);
    }
    
    del_token(tok);
  }
  
  del_lexier(l);
  for(int i = LENGTH(tests)-1; i > 0; i--) del_test_token(tests[i]);

}

void run_lexier_test() {
  printf("=== lexier ===\n");
  printf("- next token\n");
  test_next_token();
  printf("OK\n");
}
