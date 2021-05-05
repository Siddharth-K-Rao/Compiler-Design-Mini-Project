## This phase mainly focuses on:

* Evaluation of Expressions and updation in the Symbol table
* Intermediate Code Generation - Three Address Code (Quadruple format)
* Eliminate Dead code/unreachable code
* Implement Common subexpression elimination
* Implement Constant folding and Constant propagation
* Move loop invariant code outside the loop

### For execution, follow these commands:
    yacc -d parser.y
    lex lexer.l
    gcc y.tab.c lex.yy.c
    ./a.out < <Your_Input_File_Here>
