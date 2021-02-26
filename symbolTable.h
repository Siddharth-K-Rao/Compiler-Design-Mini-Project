#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAXRECST 200
#define MAXST 100

extern int yylineno;
extern int indent_depth;
extern int peek();
extern int pop();
int currentScope = 1;
int* arrayScope = NULL;

typedef struct record
{
	char* type;
	char* name;
	int decLineNo;
	int lastUseLine;
}record;

typedef struct STable
{
	int no;
	int noOfElems;
	int scope;
	record *Elements;
	int Parent;
}STable;

STable *symbolTables = NULL;
int scope_index = -1;


record* findRecord(const char *name, const char *type, int scope);
int get_hash(int base, int exp);
void updateCScope(int scope);
void resetDepth();
int scopeBasedTableSearch(int scope);
void initNewTable(int scope);
int searchRecordInScope(const char* type, const char *name, int index);
void insertRecord(const char* type, const char *name, int lineNo, int scope);
void modifyRecordID(const char *type, const char *name, int lineNo, int scope);
void checkList(const char *name, int lineNo, int scope);
void printSTable();
void freeAll();

int get_hash(int base, int exp)
{
    int ans = 1;
    int a =  base;
    int b = exp;
    while(b>0)
	{
        if(b&1)
            ans=ans*a;
        a=a*a;
        b>>=1;
    }
    return ans;
}

void updateCScope(int scope)
{
    currentScope = scope;
}

void resetDepth()
{
    while(peek()) pop();
    indent_depth = 10;
}

int scopeBasedTableSearch(int scope)
{
    int i = scope_index;
    while(i > -1)
    {
        if(symbolTables[i].scope == scope)
        {
            return i;
        }
        i--;
    }
    return -1;
}

void initNewTable(int scope)
{
    arrayScope[scope]++;
    scope_index++;
    symbolTables[scope_index].no = scope_index;
    symbolTables[scope_index].scope = get_hash(scope, arrayScope[scope]);
    symbolTables[scope_index].noOfElems = 0;
    symbolTables[scope_index].Elements = (record*)calloc(MAXRECST, sizeof(record));

    symbolTables[scope_index].Parent = scopeBasedTableSearch(currentScope);
}

int searchRecordInScope(const char* type, const char *name, int index)
{
    int i =0;
    for(i=0; i<symbolTables[index].noOfElems; i++)
    {
        if((strcmp(symbolTables[index].Elements[i].type, type)==0) && (strcmp(symbolTables[index].Elements[i].name, name)==0))
        {
            return i;
        }
    }
    return -1;
}

void modifyRecordID(const char *type, const char *name, int lineNo, int scope)
{
    int i =0;
    int index = scopeBasedTableSearch(scope);
    if(index==0)
    {
        for(i=0; i<symbolTables[index].noOfElems; i++)
        {

            if(strcmp(symbolTables[index].Elements[i].type, type)==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
            {
                symbolTables[index].Elements[i].lastUseLine = lineNo;
                return;
            }
        }
        printf("%s '%s' at line %d Not Declared\n", type, name, yylineno);
        exit(1);
    }

    for(i=0; i<symbolTables[index].noOfElems; i++)
    {
        if(strcmp(symbolTables[index].Elements[i].type, type)==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
        {
            symbolTables[index].Elements[i].lastUseLine = lineNo;
            return;
        }
    }
    return modifyRecordID(type, name, lineNo, symbolTables[symbolTables[index].Parent].scope);
}

void insertRecord(const char* type, const char *name, int lineNo, int scope)
{
    int FScope = get_hash(scope, arrayScope[scope]);
    int index = scopeBasedTableSearch(FScope);
    int recordIndex = searchRecordInScope(type, name, index);
    if(recordIndex==-1)
    {

        symbolTables[index].Elements[symbolTables[index].noOfElems].type = (char*)calloc(30, sizeof(char));
        symbolTables[index].Elements[symbolTables[index].noOfElems].name = (char*)calloc(20, sizeof(char));

        strcpy(symbolTables[index].Elements[symbolTables[index].noOfElems].type, type);
        strcpy(symbolTables[index].Elements[symbolTables[index].noOfElems].name, name);
        symbolTables[index].Elements[symbolTables[index].noOfElems].decLineNo = lineNo;
        symbolTables[index].Elements[symbolTables[index].noOfElems].lastUseLine = lineNo;
        symbolTables[index].noOfElems++;

    }
    else
    {
        symbolTables[index].Elements[recordIndex].lastUseLine = lineNo;
    }
}

void checkList(const char *name, int lineNo, int scope)
{
    int index = scopeBasedTableSearch(scope);
    int i;
    if(index==0)
    {

        for(i=0; i<symbolTables[index].noOfElems; i++)
        {

            if(strcmp(symbolTables[index].Elements[i].type, "ListTypeID")==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
            {
                symbolTables[index].Elements[i].lastUseLine = lineNo;
                return;
            }

            else if(strcmp(symbolTables[index].Elements[i].name, name)==0)
            {
                printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
                exit(1);

            }

        }
        printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
        exit(1);
    }

    for(i=0; i<symbolTables[index].noOfElems; i++)
    {
        if(strcmp(symbolTables[index].Elements[i].type, "ListTypeID")==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
        {
            symbolTables[index].Elements[i].lastUseLine = lineNo;
            return;
        }

        else if(strcmp(symbolTables[index].Elements[i].name, name)==0)
        {
            printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
            exit(1);

        }
    }

    return checkList(name, lineNo, symbolTables[symbolTables[index].Parent].scope);

}

record* findRecord(const char *name, const char *type, int scope)
{
    int i =0;
    int index = scopeBasedTableSearch(scope);
    //printf("FR: %d, %s\n", scope, name);
    if(index==0)
    {
        for(i=0; i<symbolTables[index].noOfElems; i++)
        {

            if(strcmp(symbolTables[index].Elements[i].type, type)==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
            {
                return &(symbolTables[index].Elements[i]);
            }
        }
        printf("\n%s '%s' at line %d Not Found in Symbol Table\n", type, name, yylineno);
        exit(1);
    }

    for(i=0; i<symbolTables[index].noOfElems; i++)
    {
        if(strcmp(symbolTables[index].Elements[i].type, type)==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
        {
            return &(symbolTables[index].Elements[i]);
        }
    }
    return findRecord(name, type, symbolTables[symbolTables[index].Parent].scope);
}

void printSTable()
{
    int i = 0, j = 0;

    printf("\n----------------------------All Symbol Tables----------------------------");
    printf("\nScope\tName\tType\t\tDeclaration\tLast Used Line\n");
    for(i=0; i<=scope_index; i++)
    {
        for(j=0; j<symbolTables[i].noOfElems; j++)
        {
            printf("(%d, %d)\t%s\t%s\t%d\t\t%d\n", symbolTables[i].Parent, symbolTables[i].scope, symbolTables[i].Elements[j].name, symbolTables[i].Elements[j].type, symbolTables[i].Elements[j].decLineNo,  symbolTables[i].Elements[j].lastUseLine);
        }
    }

    printf("-------------------------------------------------------------------------\n");

}

void freeAll()
{
    printf("\n");
    int i = 0, j = 0;
    for(i=0; i<=scope_index; i++)
    {
        for(j=0; j<symbolTables[i].noOfElems; j++)
        {
            free(symbolTables[i].Elements[j].name);
            free(symbolTables[i].Elements[j].type);
        }
        free(symbolTables[i].Elements);
    }
    free(symbolTables);
}