#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* maximum size of hash table */
#define SIZE 211

/* maximum size of tokens-identifiers */
#define MAXTOKENLEN 200

/* token types */
#define UNDEF "a"
#define INT_TYPE "1"
#define REAL_TYPE "2"
#define STR_TYPE "3"
#define LOGIC_TYPE "4"
#define ARRAY_TYPE "5"
#define POINTER_TYPE "6"
#define FUNCTION_TYPE "7"

/* how parameter is passed */
#define BY_VALUE 1
#define BY_REFER 2

/* parameter struct */
typedef struct Param{
	// parameter type and name
	int par_type;
	char param_name[MAXTOKENLEN];

	// to store the value
	int ival; double fval; char *st_sval;
	int passing; // value or reference
}Param;

/* a linked list of references (lineno's) for each variable */
typedef struct RefList{
    int lineno;
    struct RefList *next;
}RefList;

typedef struct list_t{
	// name, size of name, scope and occurrences (lines)
	char st_name[MAXTOKENLEN];
    int st_size;
    int scope;
    RefList *lines;

	// to store value and sometimes more information
	int st_ival; double st_fval; char *st_sval;

	// type
    char st_type[MAXTOKENLEN];

    // for arrays (info type), for pointers (pointing type)
	// and for functions (return type)
	char* inf_type;

	// array stuff
	int *i_vals; double *f_vals; char **s_vals;
	int array_size;

	// function parameters
	Param *parameters;
	int num_of_pars;

	// pointer to next item in the list
	struct list_t *next;
}list_t;

/* the hash table */
static list_t **hash_table;

// Function Declarations
void init_hash_table(); // initialize hash table
unsigned int hash(char *key); // hash function
void insert(char* type, char* name, int lineno, int len); // insert entry
list_t *lookup(char *name); // search for entry
void hide_scope(); // hide the current scope
void incr_scope(); // go to next scope
void symtab_dump();

//-----------------------------------------------Function Implementation Starts Here--------------------------------------------------------------


int cur_scope = 0;
int declare = 0; // 1: declaring, 0: not

int indent_scope = 0;

void init_hash_table(){
	int i;
	hash_table = malloc(SIZE * sizeof(list_t*));
	for(i = 0; i < SIZE; i++) hash_table[i] = NULL;
}

unsigned int hash(char *key){
	unsigned int hashval = 0;
	for(;*key!='\0';key++) hashval += *key;
	hashval += key[0] % 11 + (key[0] << 3) - key[0];
	return hashval % SIZE;
}

void insert(char *type, char *name, int lineno, int len){
	unsigned int hashval = hash(name);
	list_t *l = hash_table[hashval];
	while ((l != NULL) && (strcmp(name,l->st_name) != 0)) l = l->next;

	/* variable not yet in table */
	if (l == NULL){
		/* set up entry */
		l = (list_t*) malloc(sizeof(list_t));
		strncpy(l->st_name, name, len);
		strncpy(l->st_type,type,strlen(type));
		l->scope = cur_scope;
		l->lines = (RefList*) malloc(sizeof(RefList));
		l->lines->lineno = lineno;
		l->lines->next = NULL;

		/* add to hashtable */
		l->next = hash_table[hashval];
		hash_table[hashval] = l;
		printf("Inserted %s for the first time with linenumber %d!\n", name, lineno);
	}
	/* found in table */
	else{
		// just add line number
		if(declare == 0){
			/* find last reference */
			RefList *t = l->lines;
			while (t->next != NULL) t = t->next;

			/* add linenumber to reference list */
			t->next = (RefList*) malloc(sizeof(RefList));
			t->next->lineno = lineno;
			t->next->next = NULL;
			printf("Found %s again at line %d!\n", name, lineno);
		}
		/* new entry */
		else{
			/* same scope - multiple declaration error! */
			if(l->scope == cur_scope){
				printf("A multiple declaration of variable %s at line %d\n", name, lineno);
 				exit(1);
			}
			/* other scope - create new entry */
			else{
				/* set up entry */
				l = (list_t*) malloc(sizeof(list_t));
				strncpy(l->st_name, name, len);
				strncpy(l->st_type,type,strlen(type));
				l->scope = cur_scope;
				l->lines = (RefList*) malloc(sizeof(RefList));
				l->lines->lineno = lineno;
				l->lines->next = NULL;

				/* add to hashtable */
				l->next = hash_table[hashval];
				hash_table[hashval] = l;
				printf("Inserted %s for a new scope with linenumber %d!\n", name, lineno);
			}
		}
	}
}

list_t *lookup(char *name){ /* return symbol if found or NULL if not found */
	unsigned int hashval = hash(name);
	list_t *l = hash_table[hashval];
	while ((l != NULL) && (strcmp(name,l->st_name) != 0)) l = l->next;
	return l;
}

void hide_scope(){ /* hide the current scope */
	// list_t *l;
	// int i;
	// printf("Hiding scope \'%d\':\n", cur_scope);
	// /* for all the lists */
	// for (i = 0; i < SIZE; i++){
	// 	if(hash_table[i] != NULL){
	// 		l = hash_table[i];
	// 		/* Find the first item that is from another scope */
	// 		while(l != NULL && l->scope == cur_scope){
	// 			printf("Hiding %s..\n", l->st_name);
	// 			l = l->next;
	// 		}
	// 		/* Set the list equal to that item */
	// 		hash_table[i] = l;
	// 	}
	// }
	cur_scope--;
}

void incr_scope(){ /* go to next scope */
	cur_scope++;
}

/* print to stdout by default */
void symtab_dump(){
  int i;
  printf("------------ ------ ------ ------------\n");
  printf("Name         Type   Scope  Line Numbers\n");
  printf("------------ ------ ------ ------------\n");
  for (i=0; i < SIZE; ++i){
	if (hash_table[i] != NULL){
		list_t *l = hash_table[i];
		while (l != NULL){
			RefList *t = l->lines;
			printf("%-12s ",l->st_name);
			if (!strcmp(l->st_type,INT_TYPE)) printf("%-7s","int");
			else if (!strcmp(l->st_type,REAL_TYPE)) printf("%-7s","real");
			else if (!strcmp(l->st_type,STR_TYPE)) printf("%-7s","string");
            /*
			else if (l->st_type == ARRAY_TYPE){
				printf(of,"array of ");
				if (l->inf_type == INT_TYPE) 		   printf(of,"%-7s","int");
				else if (l->inf_type  == REAL_TYPE)    printf(of,"%-7s","real");
				else if (l->inf_type  == STR_TYPE) 	   printf(of,"%-7s","string");
				else printf(of,"%-7s","undef");
			}
			else if (l->st_type == POINTER_TYPE){
				printf(of,"%-7s %s","pointer to ");
				if (l->inf_type == INT_TYPE) 		   printf(of,"%-7s","int");
				else if (l->inf_type  == REAL_TYPE)    printf(of,"%-7s","real");
				else if (l->inf_type  == STR_TYPE) 	   printf(of,"%-7s","string");
				else printf(of,"%-7s","undef");
			}
            */
			else if (!strcmp(l->st_type,FUNCTION_TYPE)){
				printf("%-7s %s","function returns ");
				if (!strcmp(l->inf_type,INT_TYPE)) 		   printf("%-7s","int");
				else if (!strcmp(l->inf_type,REAL_TYPE))    printf("%-7s","real");
				else if (!strcmp(l->inf_type,STR_TYPE)) 	   printf("%-7s","string");
				else printf("%-7s","undef");
			}
			else printf("%-7s","undef"); // if UNDEF or 0
			printf("  %d  ",l->scope);
			while (t != NULL){
				printf("%4d ",t->lineno);
			t = t->next;
			}
			printf("\n");
			l = l->next;
		}
    }
  }
}

