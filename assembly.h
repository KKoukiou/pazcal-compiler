#ifndef __ASSEMBLY_H__
#define __ASSEMBLY_H__
#include "symbol.h" 
#include "inter.h"

typedef struct str_node {
	char *str;
	struct str_node *next;
} string_node;

void Quads_to_Assembly(qnode ** head, FILE *fp); 
void getAR(quad * a, qnode *current);
/*void updateAL(SymbolEntry *p, SymbolEntry *x);*/
void load(char *R, quad *a, qnode *current);
void store(char *R, quad *a, qnode *current);
void loadAddr(char *R,quad *a, qnode *current);
char *endof(SymbolEntry *x, int q);
char *name(quad *q);
char *label(quad *q);
void print_strings(FILE *file, string_node *head);


#endif /* __ASSEMBLY_H__ */
