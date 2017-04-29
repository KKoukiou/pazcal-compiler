#include <stdio.h>
#include <stdlib.h>

#include "general.h"
#include "error.h"
#include "symbol.h"
#include "inter.h"
#include "parser.h"

void hiterror (const char *msg);

int NEXTQUAD(void)
{
	return quadNext;
};

void GENQUAD(oper op, quad *x, quad *y, quad *z)
{
	Insert(op, x, y, z, &current_quad);
	fprintf(stdout, "%d: ", quadNext); printOP(stdout, op);
	printQ(stdout, x); 
	printQ(stdout, y);
	printQ(stdout, z);
	fprintf(stdout, "\n");
	quadNext++;
};

int NEXTTEMP(Type t)
{
	SymbolEntry *se = newTemporary(t);

	return se->u.eTemporary.number;
};

void _BACKPATCH(condition **head, int q)
{
	condition *temp;

	temp = *head;
	if (temp == NULL)
		fprintf(stdout, "The list is empty\n");
	else {
		while (temp != NULL) {
			temp->n->type = QUAD_TAG;
			temp->n->value.intval = q;
			temp = temp->next;
		}
	}
};

condition *MERGE(condition **list1, condition **list2)
{
	condition *temp = *list1;

	if (*list1 == NULL)
		return *list2;
	else if (*list2 == NULL)
		return *list1;
	while (temp->next != NULL)
		temp = temp->next;
	temp->next = *list2;
	return *list1;
}

condition *MAKELIST(quad *tag, int quadnumber)
{
	condition *New;

	condition *get_cnode(quad *tag, int quadnumber);
	New = get_cnode(tag, quadnumber);
	return New;
};

condition *EMPTYLIST()
{
	condition *New = NULL;

	return New;
};

void Insert(oper op, quad *x, quad *y, quad *z, qnode **current_quad)
{
	qnode *New;

	qnode *get_qnode(oper, quad *, quad *, quad *);
	New = get_qnode(op, x, y, z);
	if (head_quad == NULL) {
		head_quad = New;
		(*current_quad) = New;
		return;
	}
	(*current_quad)->next = New;
	(*current_quad) = (*current_quad)->next;
	(*current_quad)->nestingLevel = currentScope->nestingLevel;

}

qnode *get_qnode(oper op, quad *x, quad *y, quad *z)
{
	qnode *temp;

	temp = (qnode *)malloc(sizeof(qnode));
	if (temp == NULL)
	  fprintf(stdout, "\nMemory Cannot be allocated");
	temp->n = quadNext;
	temp->op = op;
	temp->x = x;
	temp->y = y;
	temp->z = z;
	temp->next = NULL;
	return temp;
}


void DisplayQuads(qnode **head, FILE *fp)
{
	qnode *temp;
	temp = *head;
	if (temp == NULL)
			fprintf(stdout, "\nThe list is empty!\n");
	else {
			while (temp != NULL) {
				fprintf(fp, "%d: ", temp->n); printOP(fp, temp->op);
				printQ(fp, temp->x); printQ(fp, temp->y); printQ(fp, temp->z);
				fprintf(fp, "\n");
				temp = temp->next;
			}
	}
}

void DeleteQuads(qnode **head)
{
	qnode *temp;

	temp = *head;
	if (temp == NULL)
			fprintf(stdout, "\nThe list successfully deleted!\n");
	else {
			while (temp != NULL) {
				qnode *aux = temp;
				temp = temp->next;
				delete(aux);
			}
	}
}

void printQ(FILE *fp, quad *q)
{
	switch (q->type) {
	case QUAD_TOFILL:
		fprintf(fp, ", *");
		break;
	case QUAD_SE:
		fprintf(fp, ", %s",  q->value.se->id);
		break;
	case QUAD_INTEGER:
		fprintf(fp, ", %d", q->value.intval);
		break;
	case QUAD_CHAR:
		fprintf(fp, ", '%c'", (char) q->value.intval);
		break;
	case QUAD_BOOL:
		fprintf(fp, ", %d", q->value.intval);
		break;
	case QUAD_REAL:
		fprintf(fp, ", %Lf", q->value.floatval);
		break;
	case QUAD_STR:
		fprintf(fp, ", %s", q->value.strval);
		break;
	case QUAD_EMPTY:
		fprintf(fp, ", -");
		break;
	case QUAD_MODE:
		if (q->mode == PASS_BY_VALUE)
			fprintf(fp, ", V");
		else if (q->mode == PASS_BY_REFERENCE)
			fprintf(fp, ", R");
		else if (q->mode == RET)
			fprintf(fp, ", RET");
		break;
	case QUAD_TAG:
		fprintf(fp, ", %d", q->value.intval);
		break;
	case QUAD_POINTER:
		fprintf(fp, ", [%s]", q->value.se->id);
		break;	
	default:
		fprintf(fp, "Unknown quad type");
		break;
	}
}

void printOP(FILE *fp, oper op)
{
	switch (op) {
	case OP_UNIT:
		fprintf(fp, "unit");
		break;
	case OP_ENDU:
		fprintf(fp, "endu");
		break;
	case OP_JUMP:
		fprintf(fp, "jump");
		break;
	case OP_JUMPL:
		fprintf(fp, "jumpl");
		break;
	case OP_LABEL:
		fprintf(fp, "label");
		break;
	case OP_IFB:
		fprintf(fp, "ifb");
		break;
	case OP_CALL:
		fprintf(fp, "call");
		break;
	case OP_PAR:
		fprintf(fp, "par");
		break;
	case OP_RET:
		fprintf(fp, "ret");
		break;
	case OP_RETV:
		fprintf(fp, "retv");
		break;
	case OP_eq:
		fprintf(fp, "=");
		break;
	case OP_neq:
		fprintf(fp, "<>");
		break;
	case OP_less:
		fprintf(fp, "<");
		break;
	case OP_greater:
		fprintf(fp, ">");
		break;
	case OP_leq:
		fprintf(fp, "<=");
		break;
	case OP_geq:
		fprintf(fp, ">=");
		break;
	case OP_PLUS:
		fprintf(fp, "+");
		break;
	case OP_MINUS:
		fprintf(fp, "-");
		break;
	case OP_bmul:
		fprintf(fp, "*");
		break;
	case OP_bdiv:
		fprintf(fp, "/");
		break;
	case OP_bmod:
		fprintf(fp, "%%");
		break;
	case OP_mod:
		fprintf(fp, "%%");
		break;
	case OP_assign:
		fprintf(fp, ":=");
		break;
	case OP_ARRAY:
		fprintf(fp, "array");
		break;
	default:
		fprintf(fp, "UNKNOWN operation or operands\n");
		break;
	}

}



/***********		TRUE FALSE LIST IMLEMENTATIONS			****************/
condition *currentTRUE;
condition *currentFALSE;
condition *headTRUE;
condition *headFALSE;

void InsertC(quad *tag, condition **, condition **, int quadnumber);
void DisplayCList(condition **);

void
InsertC(quad *tag, condition **currentC, condition **headC, int quadnumber)
{
	condition *New;

	condition *get_cnode(quad *tag, int quadnumber);
	New = get_cnode(tag, quadnumber);
	if ((*headC) == NULL) {
		(*headC) = New;
		(*currentC) = New;
		return;
	}
	(*currentC)->next = New;
	(*currentC) = (*currentC)->next;

}

condition *get_cnode(quad *tag, int quadnumber)
{
	condition *temp;

	temp = (condition *) malloc(sizeof(condition));
	if (temp == NULL)
	  fprintf(stdout, "\nMemory Cannot be allocated");
	temp->n = tag;
	temp->tag = quadnumber;
	temp->next = NULL;
	return temp;
}


void DisplayCList(condition **head)
{
	condition *temp;

	temp = *head;
	fprintf(stdout, "DisplayCList:	");
	if (temp == NULL)
			fprintf(stdout, "\nThe list is empty!\n");
	else {
		fprintf(stdout, "{");
			while (temp != NULL) {
				fprintf(stdout, "%d,", temp->tag);
				temp = temp->next;
			}
			fprintf(stdout, "}\n");
	}
}

/******************		END TRUE FALSE LIST IMPLEMENTATIONS	*******************/

/**********		NEXT LIST HELP FUNCTIONS-STACK IMPLEMENTATION		***********/

/*stack to support nested if loops */
void Push_N(condition *Item, node_N **top)
{
	node_N *New;

	node_N *get_node_N(condition *);
	New = get_node_N(Item);
	New->next = *top;
	*top = New;
}

node_N *get_node_N(condition *item)
{
	node_N *temp;

	temp = (node_N *) malloc(sizeof(node_N));
	if (temp == NULL)
	fprintf(stdout, "\nMemory Cannot be allocated");
	temp->data = item;
	temp->next = NULL;
	return temp;
}

int Sempty_N(node_N *temp)
{
	if (temp == NULL)
	   return 1;
	else
		return 0;
}

condition *Pop_N(node_N **top)
{
	condition *item;
	node_N *temp;

	item = (*top)->data;
	temp = *top;
	*top = (*top)->next;
	free(temp);
	return item;
}


/*stack to support nested while loops*/

void Push_W(int Item, node_N **top)
{
	node_N *New;

	node_N *get_node_W(int);
	New = get_node_W(Item);
	New->next = *top;
	*top = New;
}

node_N *get_node_W(int item)
{
	node_N *temp;

	temp = (node_N *) malloc(sizeof(node_N));
	if (temp == NULL)
	fprintf(stdout, "\nMemory Cannot be allocated");
	temp->while_backQUAD = item;
	temp->next = NULL;
	return temp;
}

int Pop_W(node_N **top)
{
	int item;
	node_N *temp;

	item = (*top)->while_backQUAD;
	temp = *top;
	*top = (*top)->next;
	free(temp);
	return item;
}

void DisplayWHILE(node_N **head)
{
	node_N *temp;

	temp = *head;
	if (temp == NULL)
		fprintf(stdout, "\nThe stack is empty!");
	else {
		fprintf(stdout, "{");
		while (temp != NULL) {
			fprintf(stdout, "%d\n", temp->while_backQUAD);
			temp = temp->next;
		}
		fprintf(stdout, "}");
	}
}

/*stack to support nested for loops*/

void Push_F(int Item, SymbolEntry *se, SymbolEntry *iter, node_F **top)
{
	node_F *New;
	node_F *get_node_F(int, SymbolEntry *, SymbolEntry *);
	New = get_node_F(Item, se, iter);
	New->next = *top;
	*top = New;
}

node_F *get_node_F(int item, SymbolEntry *se, SymbolEntry *iter)
{
	node_F *temp;
	temp = (node_F *) malloc(sizeof(node_F));
	if (temp == NULL)
	  fprintf(stdout, "\nMemory Cannot be allocated");
	temp->for_backQUAD = item;
	temp->for_counter = se;
	temp->for_se = iter;
	temp->next = NULL;
	return temp;
}

node_F Pop_F(node_F **top)
{
	node_F item;
	node_F *temp;

	item = (**top);
	temp = *top;
	*top = (*top)->next;
	free(temp);
	return item;
}


/***************		END NEXT LIST HELP FUNCTIONS		   **************/

/**********************		QUADRUPLES CODE GENERATION		   **************/

void intercode_relop(Vinfo *dd, Vinfo *d1, Vinfo *d3, oper op)
{
	dd->se = NULL;

	quad *x = (quad *) new(sizeof(quad));
	quad *y = (quad *) new(sizeof(quad));
	quad *z = (quad *) new(sizeof(quad));

	dd->headTRUE = MAKELIST(z, quadNext);
	if (d1->calculated)
		switch (d1->type->kind) {
		case TYPE_INTEGER:
			x->type = QUAD_INTEGER;
			x->value.intval = d1->value;
			break;
		case TYPE_CHAR:
			x->type = QUAD_CHAR;
			x->value.intval = d1->value;
			break;
		case TYPE_REAL:
			x->type = QUAD_REAL;
			x->value.floatval = d1->floatval;
			break;
		default:
			break;
		}
	else {
		if (d1->type->kind == TYPE_POINTER)
			x->type = QUAD_POINTER;
		else 
			x->type = QUAD_SE;
		x->value.se = d1->se;
	}

	if (d3->calculated)
		switch (d3->type->kind) {
		case TYPE_INTEGER:
			y->type = QUAD_INTEGER;
			y->value.intval = d3->value;
			break;
		case TYPE_CHAR:
			y->type = QUAD_CHAR;
			y->value.intval = d3->value;
			break;
		case TYPE_REAL:
			y->type = QUAD_REAL;
			y->value.floatval = d3->floatval;
			break;
		default:
			break;
		}
	else {
		if (d3->type->kind == TYPE_POINTER)
			y->type = QUAD_POINTER;
		else 
			y->type = QUAD_SE;
		y->value.se = d3->se;
	}


	z->type = QUAD_TOFILL;
	GENQUAD(op, x, y, z);

	x = (quad *) new(sizeof(quad));
	y = (quad *) new(sizeof(quad));
	z = (quad *) new(sizeof(quad));
	dd->headFALSE = MAKELIST(z, quadNext);
	x->type = QUAD_EMPTY;
	y->type = QUAD_EMPTY;
	z->type = QUAD_TOFILL;
	GENQUAD(OP_JUMP, x, y, z);
}

void intercode_arithmetic_op(Vinfo *dd, Vinfo *d1, Vinfo *d3, oper op)
{

	quad *x = (quad *) new(sizeof(quad));

	if (d1->type->kind == TYPE_REAL && d1->calculated) {
		x->type = QUAD_REAL;
		x->value.floatval = d1->floatval;
	} else if (d1->type->kind == TYPE_INTEGER && d1->calculated) {
		x->type = QUAD_INTEGER;
		x->value.intval = d1->value;
	} else if (d1->type->kind == TYPE_CHAR && d1->calculated) {
		x->type = QUAD_CHAR;
		x->value.intval = d1->value;
	} else{
		if (d1->type->kind == TYPE_POINTER)
			x->type = QUAD_POINTER;
		else
			x->type = QUAD_SE;
		x->value.se = d1->se;
	}

	quad *y = (quad *) new(sizeof(quad));

	if (d3->type->kind == TYPE_REAL && d3->calculated) {
		y->type = QUAD_REAL;
		y->value.floatval = d3->floatval;
	} else if (d3->type->kind == TYPE_INTEGER && d3->calculated) {
		y->type = QUAD_INTEGER;
		y->value.intval = d3->value;
	} else if (d3->type->kind == TYPE_CHAR && d3->calculated) {
		y->type = QUAD_CHAR;
		y->value.intval = d3->value;
	} else {
		if (d3->type->kind == TYPE_POINTER)
			y->type = QUAD_POINTER;
		else 
			y->type = QUAD_SE;
		y->value.se = d3->se;
	}
	quad *z = (quad *) new(sizeof(quad));

	z->type = QUAD_SE;
	if (d1->type->kind == TYPE_REAL || d3->type->kind == TYPE_REAL) 
		dd->type = typeReal;
	else if ((d1->type == typePointer(typeReal)) ||
		(d3->type == typePointer(typeReal))) 
		dd->type = typeReal;
	else dd->type = typeInteger;

	SymbolEntry *se = newTemporary(dd->type);
	z->value.se = se;
	dd->se = se;

	GENQUAD(op, x, y, z);
}



void intercode_assign_op(Vinfo *d1, Vinfo *d3)
{
	if (d1->se->entryType == ENTRY_CONSTANT)
		hiterror("Cannot assign values to constant variables");	

	quad *x = (quad *) new(sizeof(quad));
	quad *y = (quad *) new(sizeof(quad));
	quad *z = (quad *) new(sizeof(quad));
	SymbolEntry *se;
	
	if (d3->type == typeBoolean) {
		if (!d3->calculated ) { 
			se = conversion_from_condition_to_expression(d3);
		} else {
		   	_BACKPATCH(&d3->headTRUE, quadNext);
			_BACKPATCH(&d3->headFALSE, quadNext);
		}
	}

	x = (quad *) new(sizeof(quad));
	y = (quad *) new(sizeof(quad));
	z = (quad *) new(sizeof(quad));

	/* If is constant or const_expr*/
	if (d3->calculated == 1) {
		switch (d3->type->kind) {
		case TYPE_REAL:
			x->type = QUAD_REAL;
			x->value.floatval = d3->floatval;
			break;
		case TYPE_INTEGER:
			x->type = QUAD_INTEGER;
			x->value.intval = d3->value;
			break;
		case TYPE_BOOLEAN:
			x->type = QUAD_BOOL;
			x->value.intval = d3->value;
			break;
		case TYPE_CHAR:
			x->type = QUAD_CHAR;
			x->value.intval = d3->value;
			break;
		case TYPE_ARRAY:
			if (d3->type->refType->kind == TYPE_CHAR) {
				x->type = QUAD_STR;
				x->value.strval = d3->strvalue;
			}
			break;
		default:
			fprintf(stdout, "Unknown constant expression type\n");
		}
	} else {
		if (d3->type->kind == TYPE_POINTER)	
			x->type = QUAD_POINTER;
		else 
			x->type = QUAD_SE;
		if (d3->type == typeBoolean)
			x->value.se = se;
		else
			x->value.se = d3->se;
	}

	y->type = QUAD_EMPTY;
	if (d1->type->kind == TYPE_POINTER)	
			z->type = QUAD_POINTER;
		else 
			z->type = QUAD_SE;
	z->value.se = d1->se;

	GENQUAD(OP_assign, x, y, z);
}


void intercode_PAR_op(SymbolEntry **current_, Vinfo *d1)
{
	/*Quadruples code*/
	SymbolEntry *current = (*current_);

	quad *x = (quad *) new(sizeof(quad));
	quad *y = (quad *) new(sizeof(quad));
	quad *z = (quad *) new(sizeof(quad));
	SymbolEntry *se;
	
	if (d1->type == typeBoolean) {
		if (d1->se == NULL) { 
			se = conversion_from_condition_to_expression(d1);
		}
	}

	/* If is constant or const_expr*/
	if (d1->calculated == 1) {
		switch (d1->type->kind) {
		case TYPE_REAL:
			x->type = QUAD_REAL;
			x->value.floatval = d1->floatval;
			break;
		case TYPE_INTEGER:
			x->type = QUAD_INTEGER;
			x->value.intval = d1->value;
			break;
		case TYPE_BOOLEAN:
			x->type = QUAD_BOOL;
			x->value.intval = d1->value;
			break;
		case TYPE_CHAR:
			x->type = QUAD_CHAR;
			x->value.intval = d1->value;
			break;
		case TYPE_ARRAY:
			if (d1->type->refType->kind == TYPE_CHAR) {
				x->type = QUAD_STR;
				x->value.strval = d1->strvalue;
			}
			break;
		default:
			fprintf(stdout, "Unknown constant expression type\n");
		}
	} else {
		if (d1->type->kind == TYPE_POINTER)	
			x->type = QUAD_POINTER;
		else 
			x->type = QUAD_SE;
		if (equalType(d1->type, typeBoolean) && d1->se == NULL)
			x->value.se = se;
		else
			x->value.se = d1->se;
	}
	/*
	y->type = QUAD_EMPTY;
	se = newTemporary(current->u.eParameter.type);
	z->type = QUAD_SE;
	z->value.se = se;
	GENQUAD(OP_assign, x, y, z);

	x = (quad *) new(sizeof(quad));
	y = (quad *) new(sizeof(quad));
	z = (quad *) new(sizeof(quad));
	
	x->type = QUAD_SE;
	x->value.se = se;
	*/
	y->type = QUAD_MODE;
	y->mode = current->u.eParameter.mode;
	z->type = QUAD_EMPTY;
	GENQUAD(OP_PAR, x, y, z);
	}

void intercode_arithmetic_op_givenRET(Vinfo *dd, Vinfo *d1, Vinfo *d3, oper op)
{

	quad *x = (quad *) new(sizeof(quad));

	if (d1->type->kind == TYPE_REAL && d1->calculated) {
		x->type = QUAD_REAL;
		x->value.floatval = d1->floatval;
	} else if (d1->type->kind == TYPE_INTEGER && d1->calculated) {
		x->type = QUAD_INTEGER;
		x->value.intval = d1->value;
	} else if (d1->type->kind == TYPE_CHAR && d1->calculated) {
		x->type = QUAD_CHAR;
		x->value.intval = d1->value;
	} else{
		if (d1->type->kind == TYPE_POINTER)
			x->type = QUAD_POINTER;
		else
			x->type = QUAD_SE;
		x->value.se = d1->se;
	}

	quad *y = (quad *) new(sizeof(quad));

	if (d3->type->kind == TYPE_REAL && d3->calculated) {
		y->type = QUAD_REAL;
		y->value.floatval = d3->floatval;
	} else if (d3->type->kind == TYPE_INTEGER && d3->calculated) {
		y->type = QUAD_INTEGER;
		y->value.intval = d3->value;
	} else if (d3->type->kind == TYPE_CHAR && d3->calculated) {
		y->type = QUAD_CHAR;
		y->value.intval = d3->value;
	} else {
		if (d3->type->kind == TYPE_POINTER)
			y->type = QUAD_POINTER;
		else 
			y->type = QUAD_SE;
		y->value.se = d3->se;
	}
	quad *z = (quad *) new(sizeof(quad));

	z->type = QUAD_SE;
	if (d1->type->kind == TYPE_REAL || d3->type->kind == TYPE_REAL) 
		dd->type = typeReal;
	else if ((d1->type == typePointer(typeReal)) ||
		(d3->type == typePointer(typeReal))) 
		dd->type = typeReal;
	else dd->type = typeInteger;

	z->value.se = dd->se;

	GENQUAD(op, x, y, z);
}




SymbolEntry *conversion_from_condition_to_expression(Vinfo *d3)
{
	fprintf(stdout, "Conversion from cond to expr\n");
	DisplayCList(&(d3->headTRUE));
	SymbolEntry *se = newTemporary(typeBoolean);

	_BACKPATCH(&d3->headTRUE, quadNext);
	quad *x = (quad *) new(sizeof(quad));
	quad *y = (quad *) new(sizeof(quad));
	quad *z = (quad *) new(sizeof(quad));

	x->type = QUAD_BOOL;
	x->value.intval = 1;
	y->type = QUAD_EMPTY;
	z->type = QUAD_SE;
	z->value.se = se;
	GENQUAD(OP_assign, x, y, z);

	x = (quad *) new(sizeof(quad));
	y = (quad *) new(sizeof(quad));
	z = (quad *) new(sizeof(quad));

	x->type = QUAD_EMPTY;
	y->type = QUAD_EMPTY;
	z->type = QUAD_TAG;
	z->value.intval = quadNext + 2;
	GENQUAD(OP_JUMP, x, y, z);

	_BACKPATCH(&d3->headFALSE, quadNext);

	x = (quad *) new(sizeof(quad));
	y = (quad *) new(sizeof(quad));
	z = (quad *) new(sizeof(quad));
	x->type = QUAD_BOOL;
	x->value.intval = 0;
	y->type = QUAD_EMPTY;
	z->type = QUAD_SE;
	z->value.se = se;
	GENQUAD(OP_assign, x, y, z);

	return se;
}

void conversion_from_expression_to_condition(Vinfo *d0, Vinfo *d1)
{
	fprintf(stdout ,"Conversion from expr to cond\n");
	quad *x ;
	quad *y ;
	quad *z ;

	if (d1->calculated) {
			/*Quadruples code*/
			x = (quad *) new(sizeof(quad));
			y = (quad *) new(sizeof(quad));
			z = (quad *) new(sizeof(quad));
			
			x->type = QUAD_EMPTY;
			y->type= QUAD_EMPTY;
			z->type= QUAD_TOFILL;
			
			if (d1->value == 1) {	
				d0->headTRUE = MAKELIST(z,quadNext);
				d0->headFALSE = EMPTYLIST();	
			} else {
				d0->headFALSE = MAKELIST(z,quadNext);
				d0->headTRUE = EMPTYLIST();	
			}
			GENQUAD(OP_JUMP,x,y,z);
			/*End Quadruples code*/	
	} else {
		x = (quad *) new(sizeof(quad));
		y = (quad *) new(sizeof(quad));		
		z = (quad *) new(sizeof(quad));
			
		x->type = QUAD_SE;
		x->value.se = d1->se;
		y->type = QUAD_EMPTY;
		z->type = QUAD_TOFILL;

		d0->headTRUE = MAKELIST(z, quadNext);
		GENQUAD(OP_IFB, x, y, z);
		DisplayCList(&d0->headTRUE);

		x = (quad *) new(sizeof(quad));
		y = (quad *) new(sizeof(quad));
		z = (quad *) new(sizeof(quad));

		x->type = QUAD_EMPTY;
		y->type= QUAD_EMPTY;
		z->type= QUAD_TOFILL;

		d0->headFALSE = MAKELIST(z, quadNext);
		GENQUAD(OP_JUMP, x, y, z);
		DisplayCList(&d0->headFALSE);	
	}

	return;
}

void initialize_array_to_zero(SymbolEntry *se)
{
	Type type = se->u.eVariable.type;
    if (type->kind != TYPE_ARRAY) {
        Vinfo a ;
        a.type = typePointer(type);
        a.se = se;

        Vinfo zero;
        zero.type = typeInteger;
        zero.calculated = 1;
        zero.value = 0;

        intercode_assign_op(&a, &zero);
		return;
    }
    for (int i=0; i<type->size; i++){
		/*Quadruples code*/
		quad *x = (quad *) new(sizeof(quad));
		quad *y = (quad *) new(sizeof(quad));
		quad *z = (quad *) new(sizeof(quad));

		y->type = QUAD_INTEGER;
		y->value.intval = i;
		
		x->type = QUAD_SE;
		x->value.se = se;
		z->type = QUAD_SE;
		z->value.se = newTemporary(type->refType);
		se = z->value.se;
		GENQUAD(OP_ARRAY, x, y, z);
		/*End Quadruples code*/
		initialize_array_to_zero(se);
    }
}


void ERROR (const char *fmt, ...);

void hiterror (const char *msg)
{
	    ERROR("%s", msg);
}

