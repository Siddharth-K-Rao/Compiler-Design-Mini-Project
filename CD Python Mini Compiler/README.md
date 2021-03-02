### For execution, follow these commands:
    yacc -d parser.y
    lex lexer.l
    gcc y.tab.c lex.yy.c
    ./a.out < <Your_Input_File_Here>
