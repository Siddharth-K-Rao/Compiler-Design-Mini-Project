### For execution, follow these commands:
    yacc -d parser.y
    <br>lex lexer.l
    <br>gcc y.tab.c lex.yy.c
    <br>./a.out < <Your_Input_File_Here>
