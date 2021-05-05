#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>


int top=-1;

typedef struct quadruples
{
char *op;
char *arg1;
char *arg2;
char *res;
int dc;
int li;
}quad;

int dc_flag = 0;
int li_flag = 0;
int while_line = -1;

int inv = 0;

int li_s = 0;

int quadlen = 0;
quad q[100];

quad q_inv[100];

char st[100][100];

char i_[2]="0";
int temp_i=0;
char tmp_i[3];
char temp[2]="t";
int label[20];
int lnum=0;
int ltop=0;
int l_while=0;
int flag_set = 1;

void push_0(char *text)
{
strcpy(st[++top], text);
}

void check_dc_li(){
    if(dc_flag==1){
        q[quadlen].dc = 1;
    }
    if(li_flag==1){
        q[quadlen].li = 1;
    }
    return;
}


void print_quads(){
    printf("\n------------------->quads before optimization<--------------------\n");
    int i = 0;
    while(i < quadlen){
        printf("%s\t\t%s\t%s\t%s\t\n", q[i].op, q[i].arg1, q[i].arg2, q[i].res);
        i++;
    }
}

void print_quads_opt(){
    printf("\n------------------->quads after optimization<--------------------\n");
    int i = 0;
    int inv_p = 0;
    while(i < quadlen){
        if(i==while_line){
            while(inv_p < inv){
            if(q_inv[inv_p].op!=NULL) printf("%s\t\t%s\t%s\t%s\t\n", q_inv[inv_p].op, q_inv[inv_p].arg1, q_inv[inv_p].arg2, q_inv[inv_p].res);
            inv_p++;
            }
        }
        if(q[i].op!=NULL) printf("%s\t\t%s\t%s\t%s\t\n", q[i].op, q[i].arg1, q[i].arg2, q[i].res);
        i++;
    }
}

void fetch_arg_value(char *arg){
    if(arg != NULL){
        if(lookup(arg)!=NULL && (strcmp(lookup(arg)->value, "null")!=0)){
            strncpy(arg, lookup(arg)->value, strlen(lookup(arg)->value));
        }
        else return;
    }
}

// void print_3ac_opt(){
//     printf("\n------------------->optimized TAC<--------------------\n");
//     int i = 0;
//     int inv_p = 0;
//     while(i < quadlen){
//         if(i==while_line){
//             while(inv_p < inv){
//             if(q_inv[i].arg1 == NULL){
//                 fetch_arg_value(arg);
//                 printf("")
//             } 
//             inv_p++;
//             }
//         }
//         printf("%s\t\t%s\t%s\t%s\t\n", q[i].op, q[i].arg1, q[i].arg2, q[i].res);
//         i++;
//     }
// }

void const_prop(){
    int i = 0;
    int inv_p = 0;
    while(i < quadlen){
        if(i==while_line){
            while(inv_p < inv){
                fetch_arg_value(q_inv[inv_p].arg1);
                fetch_arg_value(q_inv[inv_p].arg2);
                inv_p++;
            }
        }
        fetch_arg_value(q[i].arg1);
        fetch_arg_value(q[i].arg2);
        i++;
    }
}

void dead_code_elim(){
    int i = 0;
    while(i < quadlen){
        if(q[i].dc){
            q[i].op = NULL;
            q[i].arg1 = NULL;
            q[i].arg2 = NULL;
            q[i].res = NULL;
        }
        i++;
    }
}

void loop_inv(){
    int i = 0;
    while(i < quadlen){
        if(q[i].li){
            q_inv[inv].op = q[i].op;
            q_inv[inv].arg1 = q[i].arg1;
            q_inv[inv].arg2 = q[i].arg2;
            q_inv[inv].res = q[i].res;
            inv++; 
            q[i].op = NULL;
            q[i].arg1 = NULL;
            q[i].arg2 = NULL;
            q[i].res = NULL;
        }
        i++;
    }
}


void func_begin(char *name){
    check_dc_li();
    printf("func begin %s\n",name);
    q[quadlen].op = (char*)malloc(sizeof(char)*100);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(name));
    strcpy(q[quadlen].op,"func begin");
    strcpy(q[quadlen].res,name);
    quadlen++;
}

void func_end(char *name){
    check_dc_li();
    printf("func end %s\n",name);
    q[quadlen].op = (char*)malloc(sizeof(char)*100);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(name));
    strcpy(q[quadlen].op,"func end");
    strcpy(q[quadlen].res,name);
    quadlen++;
}

void param_arg(char *name, int flag){
    check_dc_li();
    if(flag == 0){
        printf("func param %s\n",name);
        q[quadlen].op = (char*)malloc(sizeof(char)*100);
        q[quadlen].arg1 = NULL;
        q[quadlen].arg2 = NULL;
        q[quadlen].res = (char*)malloc(sizeof(char)*strlen(name));
        strcpy(q[quadlen].op,"func param");
        strcpy(q[quadlen].res,name);
        quadlen++;
    }
    else{
        printf("func arg %s\n",name);
        q[quadlen].op = (char*)malloc(sizeof(char)*100);
        q[quadlen].arg1 = NULL;
        q[quadlen].arg2 = NULL;
        q[quadlen].res = (char*)malloc(sizeof(char)*strlen(name));
        strcpy(q[quadlen].op,"func arg");
        strcpy(q[quadlen].res,name);
        quadlen++;
    }
}

void func_call(char *name, int arg_count){
    check_dc_li();
    if(atoi(lookup_func(name)->value) != arg_count){
		printf("arg count not matching param count\n");
 		exit(1);
	}
    printf("call func %s, %d\n",name, arg_count);
    q[quadlen].op = (char*)malloc(sizeof(char)*100);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(name));
    strcpy(q[quadlen].op,"call func");
    strcpy(q[quadlen].res,name);
    quadlen++;
}

// void codegen1(){
//     strcpy(temp,"T");
//     sprintf(tmp_i, "%d", temp_i);
//     strcat(temp,tmp_i);
//     printf("%s = %s\n",temp, st[top]);
//     q[quadlen].op = NULL;
//     q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(st[top]));
//     q[quadlen].arg2 = NULL;
//     q[quadlen].res = (char*)malloc(sizeof(char)*strlen(temp));

//     strcpy(q[quadlen].arg1,st[top]);
//     strcpy(q[quadlen].res,temp);
    
//     quadlen++;
//     strcpy(st[top],temp);

//     temp_i++;
// }

void codegen2(char *val, int line_no)
{
    check_dc_li();
    strcpy(temp,"T");
    sprintf(tmp_i, "%d", temp_i);
    strcat(temp,tmp_i);
    insert("ICGtemporary", temp, line_no, strlen(temp), val);
    printf("%s = %s %s %s\n",temp,st[top-2],st[top-1],st[top]);
    q[quadlen].op = (char*)malloc(sizeof(char)*strlen(st[top-1]));
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(st[top-2]));
    q[quadlen].arg2 = (char*)malloc(sizeof(char)*strlen(st[top]));
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(temp));
    strcpy(q[quadlen].op,st[top-1]);
    strcpy(q[quadlen].arg1,st[top-2]);
    strcpy(q[quadlen].arg2,st[top]);
    strcpy(q[quadlen].res,temp);
    quadlen++;
    top-=2;
    strcpy(st[top],temp);

    temp_i++;
}

void cond_code_gen(int val, int line_no)
{
    check_dc_li();
    strcpy(temp,"T");
    sprintf(tmp_i, "%d", temp_i);
    strcat(temp,tmp_i);
    printf("%s = %s %s %s\n",temp,st[top-2],st[top-1],st[top]);
    char val_str[100];
    sprintf(val_str, "%d", val);
    insert("ICGtemporary", temp, line_no, strlen(temp), val_str);

    q[quadlen].op = (char*)malloc(sizeof(char)*strlen(st[top-1]));
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(st[top]));
    q[quadlen].arg2 = (char*)malloc(sizeof(char)*strlen(st[top-2]));
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(temp));
    strcpy(q[quadlen].op,st[top-1]);
    strcpy(q[quadlen].arg1,st[top]);
    strcpy(q[quadlen].arg2,st[top-2]);
    strcpy(q[quadlen].res,temp);
    quadlen++;

    top-=3;

    temp_i++;
    strcpy(st[++top],temp);
}

void codegen_umin()
{
    check_dc_li();
    printf("%s = - %s\n",st[top-2],st[top]);
    q[quadlen].op = (char*)malloc(sizeof(char));
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(st[top]));
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(st[top-2]));
    strcpy(q[quadlen].op,"-");
    strcpy(q[quadlen].arg1,st[top]);
    strcpy(q[quadlen].res,st[top-2]);
    quadlen++;
    top-=2;
}


void codegen_assign()
{
    check_dc_li();
    printf("%s = %s\n",st[top-2],st[top]);
    q[quadlen].op = (char*)malloc(sizeof(char));
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(st[top]));
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(st[top-2]));
    strcpy(q[quadlen].op,"=");
    strcpy(q[quadlen].arg1,st[top]);
    strcpy(q[quadlen].res,st[top-2]);
    quadlen++;
    top-=2;
}

void if_code_1()
{
    check_dc_li();
    lnum++;
    strcpy(temp,"T");
    sprintf(tmp_i, "%d", temp_i);
    strcat(temp,tmp_i);
    printf("%s = not %s\n",temp,st[top]);
    q[quadlen].op = (char*)malloc(sizeof(char)*4);
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(st[top]));
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(temp));
    strcpy(q[quadlen].op,"not");
    strcpy(q[quadlen].arg1,st[top]);
    strcpy(q[quadlen].res,temp);
    quadlen++;
    printf("if %s goto L%d\n",temp,lnum);
    q[quadlen].op = (char*)malloc(sizeof(char)*3);
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(temp));
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(lnum+2));
    strcpy(q[quadlen].op,"if");
    strcpy(q[quadlen].arg1,temp);
    char x[10];
    sprintf(x,"%d",lnum);
    char l[]="L";
    strcpy(q[quadlen].res,strcat(l,x));
    quadlen++;
    temp_i++;
    label[++ltop]=lnum;
}


void if_code_2()
{
    check_dc_li();
    int x;
    lnum++;
    x=label[ltop--];
    printf("goto L%d\n",lnum);
    q[quadlen].op = (char*)malloc(sizeof(char)*5);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(lnum+2));
    strcpy(q[quadlen].op,"goto");
    char jug[10];
    sprintf(jug,"%d",lnum);
    char l[]="L";
    strcpy(q[quadlen].res,strcat(l,jug));
    quadlen++;
    printf("L%d: \n",x);
    q[quadlen].op = (char*)malloc(sizeof(char)*6);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(x+2));
    strcpy(q[quadlen].op,"Label");

    char jug1[10];
    sprintf(jug1,"%d",x);
    char l1[]="L";
    strcpy(q[quadlen].res,strcat(l1,jug1));
    quadlen++;
    label[++ltop]=lnum;
}

void elif1(){
    check_dc_li();
    int x;
    lnum++;
    x=label[ltop--];
    printf("goto L%d\n",lnum + 2);
    q[quadlen].op = (char*)malloc(sizeof(char)*5);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(lnum + 2));
    strcpy(q[quadlen].op,"goto");
    char jug[10];
    sprintf(jug,"%d",lnum+2);
    char l[]="L";
    strcpy(q[quadlen].res,strcat(l,jug));
    quadlen++;
    printf("L%d: \n",x);
    q[quadlen].op = (char*)malloc(sizeof(char)*6);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(x+2));
    strcpy(q[quadlen].op,"Label");

    char jug1[10];
    sprintf(jug1,"%d",x);
    char l1[]="L";
    strcpy(q[quadlen].res,strcat(l1,jug1));
    quadlen++;
    label[++ltop]=lnum;
}

void if_code_3()
{
    check_dc_li();
    int y;
    y=label[ltop--];
    printf("L%d: \n",y);
    q[quadlen].op = (char*)malloc(sizeof(char)*6);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(y+2));
    strcpy(q[quadlen].op,"Label");
    char x[10];
    sprintf(x,"%d",y);
    char l[]="L";
    strcpy(q[quadlen].res,strcat(l,x));
    quadlen++;
    lnum++;
}

void while_code_1()
{
    check_dc_li();
    l_while = lnum;
    while_line = quadlen;
    printf("L%d: \n",lnum++);
    q[quadlen].op = (char*)malloc(sizeof(char)*6);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(lnum+2));
    strcpy(q[quadlen].op,"Label");
    char x[10];
    sprintf(x,"%d",lnum-1);
    char l[]="L";
    strcpy(q[quadlen].res,strcat(l,x));
    quadlen++;
}

void while_code_2()
{
    check_dc_li();
    strcpy(temp,"T");
    sprintf(tmp_i, "%d", temp_i);
    strcat(temp,tmp_i);
    printf("%s = not %s\n",temp,st[top]);
    q[quadlen].op = (char*)malloc(sizeof(char)*4);
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(st[top]));
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*strlen(temp));
    strcpy(q[quadlen].op,"not");
    strcpy(q[quadlen].arg1,st[top]);
    strcpy(q[quadlen].res,temp);
    quadlen++;
    printf("if %s goto L%d\n",temp,lnum);
    q[quadlen].op = (char*)malloc(sizeof(char)*3);
    q[quadlen].arg1 = (char*)malloc(sizeof(char)*strlen(temp));
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(lnum+2));
    strcpy(q[quadlen].op,"if");
    strcpy(q[quadlen].arg1,temp);
    char x[10];
    sprintf(x,"%d",lnum);char l[]="L";
    strcpy(q[quadlen].res,strcat(l,x));
    quadlen++;
    temp_i++;
 }

void while_code_3()
{
    check_dc_li();
    printf("goto L%d \n",l_while);
    q[quadlen].op = (char*)malloc(sizeof(char)*5);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(l_while+2));
    strcpy(q[quadlen].op,"goto");
    char x[10];
    sprintf(x,"%d",l_while);
    char l[]="L";
    strcpy(q[quadlen].res,strcat(l,x));
    quadlen++;
    printf("L%d: \n",lnum++);
    q[quadlen].op = (char*)malloc(sizeof(char)*6);
    q[quadlen].arg1 = NULL;
    q[quadlen].arg2 = NULL;
    q[quadlen].res = (char*)malloc(sizeof(char)*(lnum+2));
    strcpy(q[quadlen].op,"Label");
    char x1[10];
    sprintf(x1,"%d",lnum-1);
    char l1[]="L";
    strcpy(q[quadlen].res,strcat(l1,x1));
    quadlen++;
}