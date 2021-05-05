## This phase mainly focuses on:

* Lex File
* Grammar encoded in the Yacc File
* Syntax Validation
* Symbol Table Generation (Name, Type, Scope, Line Number)

### For execution, follow these commands:
    yacc -d parser.y
    lex lexer.l
    gcc y.tab.c lex.yy.c
    ./a.out < <Your_Input_File_Here>
