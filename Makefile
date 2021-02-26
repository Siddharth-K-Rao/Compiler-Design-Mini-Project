yacc -d parser.y
lex lexer.l
gcc -o steemit y.tab.c lex.yy.c
rm lex.yy.c y.tab.c
