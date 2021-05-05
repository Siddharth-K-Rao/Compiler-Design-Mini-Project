#include <stdio.h>
#define STACKSIZE 100

int stack_pointer=0;
int stack [STACKSIZE];
int indent_depth;

int peek();
void push(int indent_depth);
int pop();

int peek(){
 	if(stack_pointer>-1) return stack[stack_pointer];
 	else return 1;
}

void push(int indent_depth){
   	if(++stack_pointer < STACKSIZE) stack[stack_pointer] = indent_depth;
   	else{
        printf ("STACK FULL\n");
        exit(1);
    }
}

int pop(){
   	if(stack_pointer > -1) return stack[stack_pointer--];
   	else {
        printf ("STACK EMPTY\n");
        exit(1);
    }
}
