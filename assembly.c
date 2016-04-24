#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "assembly.h"

int size = 0;
SymbolEntry *se;
FILE *fp;
int string_no = 0;

string_node *str_list, *head_str;

void Quads_to_Assembly(qnode **head, FILE *f) {
		fp = f;
        qnode *temp;
        temp = *head;

		quad *x ;
		quad *y ;
		quad *z ;
		char *MainName;
		MainName = (char *)malloc(246);
			snprintf(MainName, 256, "_%s", main_program->id);

		fprintf(fp, "xseg\tsegment public 'code'\n"
				"\tassume\tcs:xseg, ds:xseg, ss:xseg\n"
				"\torg\t100h\n"
				"\tmain\tproc near\n"
				"\tcall\tnear ptr %s\n"
				"\tmov\tax, 4C00h\n"
				"\tint\t21h\n"
				"\tmain\tendp\n", MainName);
		free(MainName);
		fprintf(fp, //"\textrn\t_putchar:proc\n"
					//"\textrn\t_puts:proc\n"
					"\textrn\t_writeInteger:proc\n"
					"\textrn\t_writeBoolean:proc\n"
					"\textrn\t_writeChar:proc\n"
					"\textrn\t_writeReal:proc\n"
					"\textrn\t_writeString:proc\n"
					"\textrn\t_READ_INT:proc\n"
					"\textrn\t_READ_BOOL:proc\n"
					"\textrn\t_READ_CHAR:proc\n"
					"\textrn\t_READ_REAL:proc\n"
					"\textrn\t_READ_STRING:proc\n"
					"\textrn\t_abs:proc\n"
					"\textrn\t_fabs:proc\n"
					"\textrn\t_sqrt:proc\n"
					"\textrn\t_sin:proc\n"
					"\textrn\t_cos:proc\n"
					"\textrn\t_tan:proc\n"
					"\textrn\t_arctan:proc\n"
					"\textrn\t_exp:proc\n"
					"\textrn\t_ln:proc\n"
					"\textrn\t_pi:proc\n"
					"\textrn\t_trunc:proc\n"
					"\textrn\t_round:proc\n"
					"\textrn\t_TRUNC:proc\n"
					"\textrn\t_ROUND:proc\n"
					//"\textrn\t_strlen:proc\n"
					//"\textrn\t_strcmp:proc\n"
					//"\textrn\t_strcpy:proc\n"
					//"\textrn\t_strcat:proc\n"
					);
        if (temp == NULL)
                printf("\nThe list is empty!\n");
        else {
       		while (temp != NULL) {	
				fprintf(fp, "@%d:\n", temp->n);
				x = temp->x;
				y = temp->y;
				z = temp->z;
		
				switch(temp->op) {
				case OP_UNIT:
				{
					
					char *nm = name(x);
					fprintf(fp, "\t%s proc near\n"
							"\tpush\tbp\n"
							"\tmov\tbp, sp\n"
							"\tsub\tsp, %d\n", nm, -temp->negOffset);
					free(nm);
					break;
				}
				case OP_ENDU:
				{
					char *end = endof(x->value.se, temp->nestingLevel);
					fprintf(fp, "\tjmp\t%s\n", end);
					char *nm = name(x);
					fprintf(fp, "%s:\n"
								"\tmov\tsp, bp\n"
								"\tpop\tbp\n"
								"\tret\n"
								"%s\tendp\n", end, nm);
					free(nm);
					free(end);
					break;
				}
				case OP_JUMP: 
				{
					char *lbl = label(z);
					fprintf(fp, "\tjmp\t%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_JUMPL:
				{
					char *lbl = label(z);
					fprintf(fp,  "\tjmp\t%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_LABEL:
				{
					char *lbl = label(z);
					free(lbl);
					break;
				}
				case OP_IFB:
				{
					load("al", x,  temp);
					char *lbl = label(z);
					fprintf(fp,  "\tor\tal, al\n"
							"\tjnz\t%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_CALL:
				{
					/*If z is procedure*/
					if (equalType(z->value.se->u.eFunction.resultType, typeVoid))
						fprintf(fp, "\tsub\tsp, 2\n");	
					fprintf(fp, "\tsub\tsp, 2\n");
					/*updateAL(temp, z->value.se);
					*/
					char *nm = name(z);
					fprintf(fp,	"\tcall\tnear ptr %s\n"
							"\tadd\tsp, %d\n", nm, size + 4);	
					free(nm);
					size  = 0;
					break;
				}
				case OP_PAR:
				{
					if (x->type == QUAD_SE) 
						size += sizeOfType(x->value.se->u.eParameter.type);
					else if (x->type == QUAD_POINTER) 
						size += sizeOfType(x->value.se->u.eParameter.type->refType);
					else 
						size += (x->type == QUAD_INTEGER) ?
						(sizeOfType(typeInteger)) : (sizeOfType(typeChar));

					if (y->mode == PASS_BY_VALUE) {
						if (x->type == QUAD_INTEGER || (x->type == QUAD_SE &&
						equalType(x->value.se->u.eParameter.type, typeInteger))) {
							load("ax", x, temp);
							fprintf(fp, "\tpush	ax\n");
						}
						else if (x->type == QUAD_CHAR || x->type == QUAD_BOOL || 
						(x->type == QUAD_SE && 
						(equalType(x->value.se->u.eParameter.type, typeChar) ||
						equalType(x->value.se->u.eParameter.type, typeBoolean)))) {
						load("al", x, temp);
						fprintf(fp, "\tsub\tsp, 1\n"
								"\tmov\tsi, sp\n"
								"\tmov\tbyte ptr [si], al\n");
						} 
					}
					else if (y->mode == PASS_BY_REFERENCE || y->mode == RET) {
					loadAddr("si", x, temp);
						fprintf(fp, "\tpush\tsi\n");
					}
					else {
						fprintf(fp, "Error\n");
						return;
					}
					break;
				}
				case OP_RET:
				{	
						char *end = endof(temp->inFunction, temp->nestingLevel);
						fprintf(fp, "\tjmp %s\n", end);
						free(end);
					break;
				}
				case OP_RETV:
					break;
				case OP_eq:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "cmp\tax, dx\n");
					fprintf(fp, "\tje\t");
					char *lbl = label(z);
					fprintf(fp, "%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_neq:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "\tcmp\tax, dx\n");
					fprintf(fp, "\tjne\t");
					char *lbl = label(z);
					fprintf(fp, "%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_less:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "\tcmp\tax, dx\n");
					fprintf(fp, "\tjl\t");
					char *lbl = label(z);
					fprintf(fp, "%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_greater:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "\tcmp\tax, dx\n");
					fprintf(fp, "\tjg\t");
					char *lbl = label(z);
					fprintf(fp, "%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_leq:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "\tcmp\tax, dx\n");
					fprintf(fp, "\tjle\t");
					char *lbl = label(z);
					fprintf(fp, "%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_geq:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "\tcmp\tax, dx\n");
					fprintf(fp, "\tjge\t");
					char *lbl = label(z);
					fprintf(fp, "%s\n", lbl);
					free(lbl);
					break;
				}
				case OP_PLUS:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "\tadd\tax, dx\n");
					store("ax", z, temp);
					break;
				}
				case OP_MINUS:
				{
					load("ax", x, temp);
					load("dx", y, temp);
					fprintf(fp, "\tsub\tax, dx\n");
					store("ax", z, temp);
					break;
				}
				case OP_bmul:
				{
					load("ax", x, temp);
					load("cx", y, temp);
					fprintf(fp, "\timul\tcx\n");
					store("ax", z, temp);
					break;
				}
				case OP_bdiv:
				{
					load("ax", x, temp);
					fprintf(fp, "\tcwd\n");
					load("cx", y, temp);
					fprintf(fp, "\tidiv\tcx\n");
					store("ax", z, temp);
					break;
				}
				case OP_bmod:
				{
					load("ax", x, temp);
					fprintf(fp, "cwd\n");
					load("cx", y, temp);
					fprintf(fp, "\tidiv\tcx\n");
					store("dx", z, temp);
					break;
				}
				case OP_mod:
				{
					load("ax", x, temp);
					fprintf(fp, "\tcwd\n");
					load("cx", y, temp);
					fprintf(fp, "\tidiv\tcx\n");
					store("dx", z, temp);
					break;
				}
				case OP_assign:
				{
					/*Fix this*/
					if (x->type == QUAD_SE || x->type == QUAD_POINTER) {
						if (equalType(x->value.se->u.eVariable.type, typeInteger) ||
							equalType(x->value.se->u.eVariable.type, typePointer(typeInteger))) {
							load("ax", x, temp);
							store("ax", z, temp);
						} else if (equalType(x->value.se->u.eVariable.type, typeChar) ||
								   equalType(x->value.se->u.eVariable.type, typePointer(typeChar)) || 
								   equalType(x->value.se->u.eVariable.type, typeBoolean) ||
								   equalType(x->value.se->u.eVariable.type, typePointer(typeBoolean))) {
							load("al", x, temp);
							store("al", z, temp);
						}
					}
					else if (x->type == QUAD_INTEGER) {
						load("ax", x, temp);
						store("ax", z, temp);
					}	
					else if (x->type == QUAD_CHAR || x->type == QUAD_BOOL) {
						load("al", x, temp);
						store("al", z, temp);
					}
					break;
				}
				case OP_ARRAY:
				{
					load("ax", y, temp);
					fprintf(fp, "\tmov\tcx, %d\n"
							"\timul	cx\n", size);
					loadAddr("cx", x, temp);
					fprintf(fp, "\tadd\tax, cx\n");
					store("ax", z, temp);
					break;
				}
				default:
					fprintf(fp, "For debugging\n");
					break;
				}
				temp = temp->next;
			}
        }
		fprintf(fp, "xseg ends\n"
					"\tend\tmain\n");
		print_strings(fp, head_str);
}

void getAR(quad * a, qnode *current){
	int n_cur = current->nestingLevel;
	int n_a = a->value.se->nestingLevel;
	fprintf(fp, "\tmov\tsi, word ptr [bp+4]\n");
	int rep = n_cur - n_a - 1;
	int i;
	for(i=0;i<rep;++i)
		fprintf(fp, "\tmov\tsi, word ptr [si+4]\n");
}


void updateAL(SymbolEntry *p, SymbolEntry *x){
	int n_p = p->nestingLevel;
	int n_x = x->nestingLevel;
	if (n_p < n_x) {
		fprintf(fp, "push bp\n");
		return;
	}
	else if (n_p == n_x) {
		fprintf(fp, "push word ptr [bp+4]\n");
		return;
	}
	else {
		fprintf(fp, "mov si, word ptr [bp+4]\n");
		int rep = n_p - n_x-1;
		int i;
		for(i=0;i<rep;++i) 
			fprintf(fp, "mov si, word ptr [si+4]\n");
		fprintf(fp, "push word ptr [si+4]\n");
	}
}


void load(char *R, quad *a, qnode *current){
	int n_cur = current->nestingLevel;
	char *sz;
	switch (a->type){
	case QUAD_SE:
		/*Doesnt matter if its Variable or Parameter since its type is in a
		 * struct*/
		if (sizeOfType(a->value.se->u.eVariable.type) == 2)
			sz = "word";
		else if(sizeOfType(a->value.se->u.eVariable.type) == 1)
			sz = "byte";
		if(n_cur == a->value.se->nestingLevel)
			if(a->value.se->entryType == ENTRY_PARAMETER &&
				a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				fprintf(fp, "mov\tsi, word ptr [bp%d]\n",
				a->value.se->u.eParameter.offset);
				fprintf(fp, "\tmov\t%s, %s ptr [si]\n", R, sz);
			}
			else fprintf(fp, "\tmov\t%s, %s ptr [bp%d]\n", R,
				sz, a->value.se->u.eVariable.offset);
		else 
			if(a->value.se->entryType == ENTRY_PARAMETER && 
				a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				getAR(a, current);
				fprintf(fp, "\tmov\tsi, word ptr [si+%d]\n"
				, a->value.se->u.eParameter.offset);
				fprintf(fp, "\tmov\t%s, %s ptr [si]\n", R, sz);
			}
			else {
				getAR(a, current);
				fprintf(fp, "\tmov\t%s, %s ptr [si+%d]\n",
				R, sz, a->value.se->u.eParameter.offset);
			}
		break;
	case QUAD_INTEGER:
		fprintf(fp, "\tmov\t%s, %d\n", R, a->value.intval);
		break;
	case QUAD_CHAR:
		fprintf(fp, "\tmov\t%s, %d\n", R, a->value.intval);
		break;
	case QUAD_BOOL:
		fprintf(fp, "\tmov\t%s, %d\n", R, a->value.intval);
		break;
	/*case QUAD_REAL:
		fprintf(fp, ", %Lf", a->value.floatval);
		break;
	case QUAD_STR:
		fprintf(fp, ", %s", a->value.strval);
		break;
	*/
	case QUAD_POINTER:
		a->type = QUAD_SE;
		load("di", a, current);
		if (sizeOfType(a->value.se->u.eVariable.type->refType) == 2)
			sz = "word";
		else if(sizeOfType(a->value.se->u.eVariable.type->refType) == 1)
			sz = "byte";
		fprintf(fp, "\tmov\t%s, %s ptr[di]\n", R, sz);
		break;
	default:
		break;
	}

}


void store(char *R, quad *a, qnode *current){
	int n_cur = current->nestingLevel;
	char *sz;
	switch (a->type){
	case QUAD_SE:
		if (sizeOfType(a->value.se->u.eVariable.type) == 2)
			sz = "word";
		else if(sizeOfType(a->value.se->u.eVariable.type) == 1)
			sz = "byte";
		if(n_cur == a->value.se->nestingLevel) {
			if(a->value.se->entryType == ENTRY_PARAMETER &&
				a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				fprintf(fp, "\tmov\tsi, word ptr [bp+%d]\n",
				a->value.se->u.eParameter.offset);
				fprintf(fp, "\tmov\t%s ptr [si], %s\n", sz, R);
			}
			else fprintf(fp, "\tmov\t%s ptr [bp%d], %s\n",
				sz, a->value.se->u.eVariable.offset, R);
		}
		else {
			if(a->value.se->entryType == ENTRY_PARAMETER && 
				a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				getAR(a, current);
				fprintf(fp, "\tmov\tsi, word ptr [si+%d]\n"
				, a->value.se->u.eParameter.offset);

				fprintf(fp, "\tmov\t%s ptr [si], %s\n", sz, R);
			}
			else {
				getAR(a, current);
				fprintf(fp, "\tmov\t%s ptr [si+%d], %s\n",
				sz, a->value.se->u.eParameter.offset, R);
			}
		}
		break;
	case QUAD_POINTER:
		a->type = QUAD_SE;
		store("di", a, current);
		if (sizeOfType(a->value.se->u.eVariable.type->refType) == 2)
			sz = "word";
		else if(sizeOfType(a->value.se->u.eVariable.type->refType) == 1)
			sz = "byte";
		fprintf(fp, "\tmov\t%s ptr[di], %s\n", sz, R);
		break;
	default:
		break;
	}
}


void loadAddr(char *R, quad *a, qnode *current){
	int n_cur = current->nestingLevel;
	char *sz;
	switch (a->type){
	case QUAD_STR:
		if (string_no == 0) {
			str_list =  malloc( sizeof(string_node) ); 
			head_str = str_list;
		}
		else {
			str_list->next =  malloc( sizeof(string_node) ); 
			str_list = str_list->next;
		}
		str_list->next = NULL;
		str_list->str = a->value.strval; 
		fprintf(fp, "\tlea\t%s, byte ptr @str%d\n", R, string_no);
		string_no++;
		break;
	case QUAD_SE:
		if (sizeOfType(a->value.se->u.eVariable.type) == 2)
			sz = "word";
		else if (sizeOfType(a->value.se->u.eVariable.type) == 1)
			sz = "byte";
		if(n_cur == a->value.se->nestingLevel)
			if(a->value.se->entryType == ENTRY_PARAMETER && a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				fprintf(fp, "\tmov\t%s, word ptr [bp%d]\n", R, a->value.se->u.eParameter.offset);
			}
			else fprintf(fp, "\tlea\t%s, %s ptr [bp%d]\n", R,
				sz, a->value.se->u.eVariable.offset);
		else 
			if(a->value.se->entryType == ENTRY_PARAMETER && a->value.se->u.eParameter.mode == PASS_BY_REFERENCE){
				getAR(a, current);
				fprintf(fp, "\tmov\t%s, word ptr [si+%d]\n", R,
				a->value.se->u.eParameter.offset);
			}
			else getAR(a, current), fprintf(fp, "\tlea\t%s, %s ptr [si+%d]\n", R,
				sz, a->value.se->u.eVariable.offset);
		break;
	case QUAD_POINTER:
		load(R, a, current);
		break;
	default:
		printf("Could not loadAddr for such type\n");
	}
}
char *endof(SymbolEntry *x, int n)
{
	char *buf;
	buf = (char *)malloc(256);
	snprintf(buf, 256, "@%s_%d", x->id, n);
	return buf;
}

char *name(quad *q)
{
	char *buf;
	buf = (char *)malloc(246);
	if (q->type == QUAD_SE)
		snprintf(buf, 256, "_%s", q->value.se->id);
	return buf;
}

char *label(quad *q)
{
	char *buf;
	buf = (char *)malloc(246);
	if (q->type == QUAD_TAG)
		snprintf(buf, 256, "@%d", q->value.intval);
	return buf;
}

void print_strings(FILE *fp, string_node *head) {
	string_node *current = head;
	int c = 0;
	while (current != NULL) {
		fprintf(fp, "@str%d\tdb %s\n"
					"\tdb 0\n", c, current->str);
		current = current->next;
		c++;
	}
}
