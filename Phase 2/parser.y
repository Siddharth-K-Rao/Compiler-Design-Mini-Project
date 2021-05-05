%{
	#include <stdio.h>
	#include <stdarg.h>
	#include "symTab_new.h"
    #include "tac.h"

	int yylex();
	void yyerror(const char *msg);
    extern int yylineno; 

    int count = 0;

    //char val_temp[200];
    char func_name[100];

    char identifier[100];

    int op_count = 0;

    char func_call_name[100];

    int arg_count = 0;
    int param_count = 0;
    char param_count_str[10];
    
    int func_line_no;

	void finish()
	{
		printf("\n------------------->valid python syntax<--------------------\n\n\n\n");
        print_symbol_table();
        print_quads();
        dead_code_elim();
        loop_inv();
        const_prop();
        print_quads_opt();
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
%type<data> arith_exp term constant list_index

%%

Start : StartParse T_EOF {finish();};

constant : T_number {
                    insert("Constant", $<data>1, @1.first_line, strlen($<data>1), "null");
                    }
         | T_string {insert("Constant", $<data>1, @1.first_line, strlen($<data>1), "null");}
		 | T_None {insert("Constant", "None", @1.first_line, strlen($<data>1), "null");};

term : T_identifier {
                        push_0($<data>1);
                        check_scope($<data>1, @1.first_line);
                        if((strcmp(identifier, $<data>1) != 0) && (li_s)){ li_flag ++; }
                        else li_flag -- ;
                        insert("Identifier", $<data>1, @1.first_line, strlen($<data>1), "null");
                        strncpy($$, lookup($<data>1)->value, strlen(lookup($<data>1)->value));
                    }
     | constant {if((strcmp(identifier, $<data>1) != 0) && (li_s)) li_flag ++ ; $$ = $<data>1; push_0($<data>1);}
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

arith_exp : term {if(li_s) op_count++;$$=$<data>1;}
          | arith_exp T_plus {push_0("+");}  arith_exp {
                                            int val = atoi($1) + atoi($4);
                                            sprintf($$, "%d", val);
                                            codegen2($$, @1.first_line);
                                          }
          | arith_exp T_minus {push_0("-");}  arith_exp {
                                            int val = atoi($1) - atoi($4);
                                            sprintf($$, "%d", val);
                                            codegen2($$, @1.first_line);
                                        }
          | arith_exp T_multiply {push_0("*");}  arith_exp {
                                            int val = atoi($1) * atoi($4);
                                            sprintf($$, "%d", val);
                                            codegen2($$, @1.first_line);
                                        }
          | arith_exp T_division {push_0("/");}  arith_exp{
                                            int val = atoi($1) / atoi($4);
                                            sprintf($$, "%d", val);
                                            codegen2($$, @1.first_line);
                                        }
          | T_openParanthesis arith_exp T_closeParanthesis{
                                            $$ = $2;
                                        };


bool_exp : bool_term T_or bool_term
         | arith_exp T_lesserThan {push_0("<");} arith_exp {int val = atoi($1) < atoi($4); cond_code_gen(val, @1.first_line);}
         | bool_term T_and bool_term
         | arith_exp T_greaterThan {push_0(">");} arith_exp {int val = atoi($1) > atoi($4); cond_code_gen(val, @1.first_line);}
         | arith_exp T_lesserThanEqualTo {push_0("<=");} arith_exp {int val = atoi($1) <= atoi($4); cond_code_gen(val, @1.first_line);}
         | arith_exp T_greaterThanEqualTo {push_0(">=");} arith_exp {int val = atoi($1) >= atoi($4); cond_code_gen(val, @1.first_line);}
         | arith_exp T_in {push_0("in");} T_identifier
         | bool_term ;

bool_term : bool_factor 
          | arith_exp T_equalOP arith_exp
          | T_True {insert("Constant", "True", @1.first_line, strlen($<data>1), "null");}
          | T_False {insert("Constant", "False", @1.first_line, strlen($<data>1), "null");};

bool_factor : T_not bool_factor
            | T_openParanthesis bool_exp T_closeParanthesis;

import_stmt : T_import T_identifier {insert("Package name", $<data>2, @2.first_line, strlen($<data>2), "null");};
pass_stmt : T_pass
continue_stmt : T_continue
break_stmt : T_break
return_stmt : T_return {dc_flag = 1;}


assign_stmt : T_identifier {strncpy(identifier, $<data>1, strlen($<data>1));push_0($<data>1);} T_assignOP {push_0("=");} assign;


assign: arith_exp{
                    insert("Identifier", identifier, @1.first_line, strlen(identifier), $1);
                    if(op_count==1) li_flag = 1;
                    codegen_assign();
                    li_flag = 0;
                    op_count = 0;
                }
      |T_minus arith_exp{
                            char temp[100] = "-";
                            strcat(temp, $2);
                            insert("Identifier", identifier, @1.first_line, strlen(identifier), temp);
                            if(op_count==1) li_flag = 1;
                            codegen_umin();
                            li_flag = 0;
                            op_count = 0;
                        }
      | bool_exp {insert("Identifier", $<data>1, @1.first_line, strlen($<data>1), "null");}
      | func_call {insert("List identifier", $<data>1, @1.first_line, strlen($<data>1), "null");}
      | T_openBracket list_elements T_closeBracket {insert("List identifier", $<data>1, @1.first_line, strlen($<data>1), "null");};     

print_stmt : T_print T_openParanthesis term T_closeParanthesis

finalStatements : basic_stmt
                | cmpd_stmt
                | func_def
                | func_call
                | error T_newLine {yyerrok; yyclearin;};

cmpd_stmt : if_stmt
          | while_stmt;

if_stmt : T_if bool_exp {if_code_1();} T_colon start_suite if_opt;

if_opt: elif_stmts
      | %prec T_if; 		

elif_stmts : else_stmt
           | T_elif {elif1();count++;} bool_exp {if_code_1();} T_colon start_suite {if_code_2();} elif_stmts;

else_stmt : T_else {if(count == 0){ count = 0; if_code_2();}} T_colon start_suite;

func_def : T_def T_identifier {strncpy(func_name,$<data>2,strlen($<data>2)); func_begin($<data>2); func_line_no=@2.first_line;} T_openParanthesis{flag = 1; func_no++;} param T_closeParanthesis T_colon start_suite_func

func_call : T_identifier {strncpy(func_call_name,$<data>1,strlen($<data>1)); insert("Func_Name", $<data>1, @1.first_line, strlen($<data>1), "null");} T_openParanthesis list_elements T_closeParanthesis {func_call(func_call_name, arg_count);arg_count=0;}

while_stmt : T_while {while_code_1();} bool_exp {li_s = 1; while_code_2();} T_colon start_suite_while;

start_suite_while : basic_stmt
            | T_newLine Indent finalStatements suite_while;

suite_while : T_newLine Nodent finalStatements suite_while
      | T_newLine end_suite_while;

end_suite_while : Dedent finalStatements
          | Dedent
          | {li_s = 0; while_code_3();reset_depth();} finalStatements
          | {li_s = 0; while_code_3(); reset_depth();};

start_suite_func : basic_stmt
            | T_newLine Indent finalStatements suite_func;

suite_func : T_newLine Nodent finalStatements suite_func
      | T_newLine end_suite_func;

end_suite_func : Dedent finalStatements
          | Dedent
          | {dc_flag = 0;func_end(func_name); sprintf(param_count_str, "%d", param_count); insert("Func_Name", func_name, func_line_no, strlen(func_name), param_count_str); flag = 0; cur_scope = 0;reset_depth();param_count=0;} finalStatements
          | {dc_flag = 0;func_end(func_name); sprintf(param_count_str, "%d", param_count); insert("Func_Name", func_name, func_line_no, strlen(func_name), param_count_str); reset_depth();param_count=0;};

start_suite : basic_stmt
            | T_newLine Indent finalStatements suite;

suite : T_newLine Nodent finalStatements suite
      | T_newLine end_suite;
      | {reset_depth();} elif_stmts;

end_suite : Dedent finalStatements
          | Dedent
          | {if_code_3();reset_depth();} finalStatements
          | {if_code_3();reset_depth();};

param : T_identifier {param_arg($<data>1, 0); insert("Identifier", $<data>1, @1.first_line, strlen($<data>1), "null");param_count++;} params | ;

params : T_comma T_identifier {param_arg($<data>2, 0); insert("Identifier", $<data>2, @1.first_line, strlen($<data>2), "null");param_count++;} params | ;

list_element : T_comma term {param_arg($<data>2, 1);arg_count++;} list_element | ;

list_elements : T_identifier {param_arg($<data>1, 1);arg_count++ ;insert("Identifier", $<data>1, @1.first_line, strlen($<data>1), "null");} list_element | T_number {param_arg($<data>1, 1); insert("Constant", $<data>1, @1.first_line, strlen($<data>1), "null");arg_count++;} list_element | T_string {param_arg($<data>1, 1); insert("Constant", $<data>1, @1.first_line, strlen($<data>1), "null");arg_count++;} list_element |


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
