#ifndef __cplusplus 
#include <stdio.h>
#include <stdlib.h>
#else
#include<iostream>
#include<string>
using namespace std;
#endif

#include "mpc.h"

#ifdef _WIN32

#include <string.h>
static char buffer[2048];
#ifndef __cplusplus
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}
#endif
void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif
/* Use operator string to see which operation to perform */
long eval_op(long x, char*op,long y)
{
	if(strcmp(op,"+")==0){return x+y;
	}
	if(strcmp(op,"-")==0){return x-y;
	}
	if(strcmp(op,"*")==0){return x*y;
	}
	if(strcmp(op,"/")==0){return x/y;
	}
	return 0;
}

long eval(mpc_ast_t*t){
	/* If tagged as number return it directly. */
	if(strstr(t->tag,"number"))
		return atoi(t->contents);
	else
	{
		/* The operator is always second child. */
		char* op=t->children[1]->contents;
		
		/* We store the third child in 'x' */ 
		long x=eval(t->children[2]);
		
		/* Iterate the remaining children and combining. */
		int i=3;
		while(strstr(t->children[i]->tag,"expr")){
			x=eval_op(x,op,eval(t->children[i]));
			i++;
		}
	return x;
	}
}
int main(int argc, char** argv) {

  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);

  puts("Lispy Version 0.0.0.0.3");
  puts("Press Ctrl+c to Exit\n");

  while (1) {
	    mpc_result_t r;
	 #ifndef __cplusplus
	    char* input = readline("lispy> ");
	    add_history(input);
	
	    /* Attempt to parse the user input */
	    if (mpc_parse("<stdin>", input, Lispy, &r)) {
	      /* On success print and delete the AST */
	      //mpc_ast_print(r.output);
	      long result=eval(r.output);
	      printf("%li\n",result);
	      mpc_ast_delete(r.output);
	    } else {
	      /* Otherwise print and delete the Error */
	      mpc_err_print(r.error);
	      mpc_err_delete(r.error);
	    }
	    free(input);
	 #else
	 	string input;
		cout<<"lispy>";
		getline(cin,input);
    	add_history((char*)input.data());
	    if (mpc_parse("<stdin>", (char*)input.data(), Lispy, &r)) {
	      /* On success print and delete the AST */
	      //mpc_ast_print(r.output);
	      long result=eval((mpc_ast_t*)r.output);
	      printf("%li\n",result);
	      mpc_ast_delete((mpc_ast_t*)r.output);
	    } else {
	      /* Otherwise print and delete the Error */
	      mpc_err_print(r.error);
	      mpc_err_delete(r.error);
	    }
	 #endif    
  }

  /* Undefine and delete our parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
