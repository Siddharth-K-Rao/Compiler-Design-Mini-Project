%{
	#include <stdio.h>
	#include <stdarg.h>
	#include "symTab_new.h"

	int yylex();
	void yyerror(const char *msg);
    extern int yylineno;

	void finish()
	{
		printf("\n------------------->valid python syntax<--------------------\n\n\n\n");
        	print_symbol_table();
		exit(0);
	}

%}

%union {char *data; int indent_depth;};

%token T_identifier T_False T_True T_number Indent Nodent Dedent T_string T_print T_colon T_newLine T_lesserThan T_greaterThanEqualTo T_lesserThanEqualTo T_or T_and T_not T_assignOP T_notEqualOP T_equalOP T_greaterThan T_in T_if T_elif T_while T_else T_import T_break T_pass T_minus T_plus T_division T_multiply T_openParanthesis T_closeParanthesis T_EOF T_return T_openBracket T_closeBracket T_def T_comma T_List T_range T_None T_continue

%left T_plus T_minus
%left T_multiply T_division
%right T_assignOP
%nonassoc T_if
%nonassoc T_elif
%nonassoc T_else
%start Start

%%

Start : StartParse T_EOF {finish();};

constant : T_number {insert("Constant", $<data>1, @1.first_line, strlen($<data>1));}
         | T_string {insert("Constant", $<data>1, @1.first_line, strlen($<data>1));}
		 | T_None {insert("Constant", "None", @1.first_line, strlen($<data>1));};

term : T_identifier {check_scope($<data>1, @1.first_line); insert("Identifier", $<data>1, @1.first_line, strlen($<data>1));}
     | constant
     | list_index;


list_index : T_identifier T_openBracket constant T_closeBracket {check_scope($<data>1, @1.first_line); is_iter($<data>1, @1.first_line);};

StartParse : T_newLine StartParse | finalStatements T_newLine {reset_depth();} StartParse | ;

basic_stmt : pass_stmt
           | break_stmt
		   | continue_stmt
           | import_stmt
           | assign_stmt
           | arith_exp
           | bool_exp
           | print_stmt
           | return_stmt;

arith_exp : term
          | arith_exp  T_plus  arith_exp
          | arith_exp  T_minus  arith_exp
          | arith_exp  T_multiply  arith_exp
          | arith_exp  T_division  arith_exp
          | T_minus arith_exp
          | T_openParanthesis arith_exp T_closeParanthesis;


bool_exp : bool_term T_or bool_term
         | arith_exp T_lesserThan arith_exp
         | bool_term T_and bool_term
         | arith_exp T_greaterThan arith_exp
         | arith_exp T_lesserThanEqualTo arith_exp
         | arith_exp T_greaterThanEqualTo arith_exp
         | arith_exp T_in T_identifier
         | bool_term ;

bool_term : bool_factor
          | arith_exp T_equalOP arith_exp
          | T_True {insert("Constant", "True", @1.first_line, strlen($<data>1));}
          | T_False {insert("Constant", "False", @1.first_line, strlen($<data>1));};

bool_factor : T_not bool_factor
            | T_openParanthesis bool_exp T_closeParanthesis;

import_stmt : T_import T_identifier {insert("Package name", $<data>2, @2.first_line, strlen($<data>2));};
pass_stmt : T_pass
continue_stmt : T_continue
break_stmt : T_break
return_stmt : T_return

assign_stmt : T_identifier T_assignOP arith_exp {insert("Identifier", $<data>1, @1.first_line, strlen($<data>1));}
            | T_identifier T_assignOP bool_exp {insert("Identifier", $<data>1, @1.first_line, strlen($<data>1));}
            | T_identifier  T_assignOP func_call {insert("Identifier", $<data>1, @1.first_line, strlen($<data>1));}
            | T_identifier T_assignOP T_openBracket list_elements T_closeBracket {insert("List identifier", $<data>1, @1.first_line, strlen($<data>1));}
			| T_identifier T_assignOP T_range {insert("List identifier", $<data>1, @1.first_line, strlen($<data>1));} T_openParanthesis range_args T_closeParanthesis ;

print_stmt : T_print T_openParanthesis term T_closeParanthesis

finalStatements : basic_stmt
                | cmpd_stmt
                | func_def
                | func_call
                | error T_newLine {yyerrok; yyclearin;};

cmpd_stmt : if_stmt
          | while_stmt;


if_stmt : T_if bool_exp T_colon start_suite 		%prec T_if
        | T_if bool_exp T_colon start_suite elif_stmts;

elif_stmts : else_stmt
           | T_elif bool_exp T_colon start_suite elif_stmts;

else_stmt : T_else T_colon start_suite;

func_def : T_def T_identifier {insert("Func_Name", $<data>2, @2.first_line, strlen($<data>2));} T_openParanthesis{flag = 1; func_no++;} param T_closeParanthesis T_colon start_suite

func_call : T_identifier {insert("Func_Name", $<data>1, @1.first_line, strlen($<data>1));} T_openParanthesis list_elements T_closeParanthesis

range_args : T_number T_comma T_number T_comma T_number {insert("Constant", $<data>1, @1.first_line, strlen($<data>1)); insert("Constant", $<data>3, @3.first_line, strlen($<data>3)); insert("Constant", $<data>5, @5.first_line, strlen($<data>5));}
		   | T_number T_comma T_number {insert("Constant", $<data>1, @1.first_line, strlen($<data>1)); insert("Constant", $<data>3, @3.first_line, strlen($<data>3));}
		   | T_number {insert("Constant", $<data>1, @1.first_line, strlen($<data>1));};

while_stmt : T_while bool_exp T_colon start_suite;

start_suite : basic_stmt
            | T_newLine Indent finalStatements suite;

suite : T_newLine Nodent finalStatements suite
      | T_newLine end_suite;
      | {reset_depth();} elif_stmts;

end_suite : Dedent finalStatements
          | Dedent
          | {flag = 0; cur_scope = 0;reset_depth();} finalStatements
          | {reset_depth();};

param : T_identifier {insert("Identifier", $<data>1, @1.first_line, strlen($<data>1));} params | ;

params : T_comma T_identifier {insert("Identifier", $<data>2, @1.first_line, strlen($<data>2));} params | ;

list_element : T_comma term list_element | ;

list_elements : T_identifier {insert("Identifier", $<data>1, @1.first_line, strlen($<data>1));} list_element | T_number {insert("Constant", $<data>1, @1.first_line, strlen($<data>1));} list_element | T_string {insert("Constant", $<data>1, @1.first_line, strlen($<data>1));} list_element |


%%

void yyerror(const char *msg)
{
	printf("\nSyntax Error at Line %d, Column : %d\n",  yylineno, yylloc.last_column);
	exit(0);
}

int main()
{
    init_hash_table();
	yyparse();
	return 0;
}
