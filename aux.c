#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void printSymbolTable(void)
{
	Scope *scp;
	SymbolEntry *e;
	SymbolEntry *args;

	scp = currentScope;
	if (scp == NULL)
		fprintf(stdout, "no scope\n");
	else
		while (scp != NULL) {
			fprintf(stdout, "scope: ");
			e = scp->entries;
			while (e != NULL) {
				if (e->entryType == ENTRY_TEMPORARY)
					fprintf(stdout, "$%d", e->u.eTemporary.number);
				else
					fprintf(stdout, "%s", e->id);
				switch (e->entryType) {
				case ENTRY_FUNCTION:
					fprintf(stdout, "(");
					args = e->u.eFunction.firstArgument;
					while (args != NULL) {
						printMode(args->u.eParameter.mode);
						fprintf(stdout, "%s : ", args->id);
						printType(args->u.eParameter.type);
						args = args->u.eParameter.next;
						if (args != NULL)
							fprintf(stdout, "; ");
					}
					fprintf(stdout, ") : ");
					printType(e->u.eFunction.resultType);
					break;
#ifdef SHOW_OFFSETS
				case ENTRY_VARIABLE:
					fprintf(stdout, "[%d]", e->u.eVariable.offset);
					break;
				case ENTRY_PARAMETER:
					fprintf(stdout, "[%d]", e->u.eParameter.offset);
					break;
				case ENTRY_TEMPORARY:
					fprintf(stdout, "[%d]", e->u.eTemporary.offset);
					break;
#endif
				}
				e = e->nextInScope;
				if (e != NULL)
					fprintf(stdout, ", ");
		 }
			scp = scp->parent;
		fprintf(stdout, "\n");
		}
		fprintf(stdout, "----------------------------------------\n");
}




/***************	COMPATIBILITY FUNCTIONS			******************/

void compatible_PASS_BY_REFERENCE(Type type1, Type type2)
//type 1 : pragmatiki parametros
//type 2 : typiki parametros
{
	switch (type1->kind) {
	case TYPE_INTEGER:
		switch (type2->kind) {
		case TYPE_REAL:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_INTEGER:
			break;
		case TYPE_BOOLEAN:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_CHAR:
			break;
		case TYPE_ARRAY:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_IARRAY:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_POINTER:
			compatible_PASS_BY_REFERENCE(type1, type2->refType);
			break;
		default:
			break;
		}
		break;
	case TYPE_REAL:
		switch (type2->kind) {
		case TYPE_REAL:
			break;
		case TYPE_INTEGER:
			break;
		case TYPE_BOOLEAN:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_CHAR:
			break;
		case TYPE_ARRAY:
			yyerror("Parameter mismatch in redeclaration. ");
			break;
		case TYPE_IARRAY:
			yyerror("Parameter mismatch in redeclaration. ");
			break;
		case TYPE_POINTER:
			compatible_PASS_BY_REFERENCE(type1, type2->refType);
			break;
		default:
			break;
		}
		break;
	case TYPE_BOOLEAN:
		switch (type2->kind) {
		case TYPE_REAL:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_INTEGER:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_BOOLEAN:
			break;
		case TYPE_CHAR:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_ARRAY:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_IARRAY:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_POINTER:
			compatible_PASS_BY_REFERENCE(type1, type2->refType);
			break;
		default:
			break;
		}
		break;
	case TYPE_CHAR:
		switch (type2->kind) {
		case TYPE_REAL:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_INTEGER:
			break;
		case TYPE_BOOLEAN:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_CHAR:
			break;
		case TYPE_ARRAY:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_IARRAY:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_POINTER:
			compatible_PASS_BY_REFERENCE(type1, type2->refType);
			break;
		default:
			break;
		}
		break;
	case TYPE_ARRAY:
		switch (type2->kind) {
		case TYPE_REAL:
			yyerror("Parameter mismatch in redeclaration.");
		break;
		case TYPE_INTEGER:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_BOOLEAN:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_CHAR:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_ARRAY:
			compatible_PASS_BY_REFERENCE(type1->refType, type2->refType);
			break;
		case TYPE_IARRAY:
			yyerror("Parameter mismatch in redeclaration.");
			break;
		case TYPE_POINTER:
			compatible_PASS_BY_REFERENCE(type1, type2->refType);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}



void compatible_assignment(Type type1, Type type2)
{
	switch (type1->kind) {
	case TYPE_INTEGER:
		switch (type2->kind) {
		case TYPE_REAL:
			yyerror("Type mismatch. cannot assign a real to an integer ");
			break;
		case TYPE_INTEGER:
			break;
		case TYPE_BOOLEAN:
			yyerror("Type mismatch. cannot assign a boolean to an integer ");
			break;
		case TYPE_CHAR:
			break;
		case TYPE_POINTER:
			compatible_assignment(type1, type2->refType);
			break;
		default:
			yyerror("Type mismatch. cannot assign table to an integer");
			break;
		}
		break;
	case TYPE_REAL:
		switch (type2->kind) {
		case TYPE_REAL:
			break;
		case TYPE_INTEGER:
			break;
		case TYPE_BOOLEAN:
			yyerror("Type mismatch. cannot assign a boolean to a real ");
			break;
		case TYPE_CHAR:
			break;
		case TYPE_POINTER:
			compatible_assignment(type1, type2->refType);
			break;
		default:
			yyerror("Type mismatch. cannot assign table to a real");
			break;
		}
		break;
	case TYPE_BOOLEAN:
		switch (type2->kind) {
		case TYPE_REAL:
			yyerror("Type mismatch. cannot assign a real to a boolean ");
			break;
		case TYPE_INTEGER:
			yyerror("Type mismatch. cannot assign an integer to a boolean ");
			break;
		case TYPE_BOOLEAN:
			break;
		case TYPE_CHAR:
			yyerror("Type mismatch. cannot assign a char to a boolean ");
			break;
		case TYPE_POINTER:
			compatible_assignment(type1, type2->refType);
			break;
		default:
			yyerror("Type mismatch. cannot assign a table to a boolean ");
			break;
		}
		break;
	case TYPE_CHAR:
		switch (type2->kind) {
		case TYPE_REAL:
			yyerror("Type mismatch. cannot assign a real to a char ");
			break;
		case TYPE_INTEGER:
			break;
		case TYPE_BOOLEAN:
			yyerror("Type mismatch. cannot assign a boolean to a char ");
			break;
		case TYPE_CHAR:
			return;
			break;
		case TYPE_POINTER:
			compatible_assignment(type1, type2->refType);
			break;
		default:
			yyerror("Type mismatch. cannto assign a table to a char ");
			break;
		}
		break;
	case TYPE_POINTER:
			compatible_assignment(type1->refType, type2);
			break;
	default:
		yyerror("Type mismatch. while procecing table assignment");
		break;
	}

	}


Type compatible_arithmetic_OP(Type type1, Type type2)
{
	switch (type1->kind) {
	case TYPE_INTEGER:
		switch (type2->kind) {
		case TYPE_REAL:
			return typeReal;
		case TYPE_INTEGER:
			return typeInteger;
		case TYPE_BOOLEAN:
			yyerror("Type mismatch. Invalid operands for arithmetic operation ");
			break;
		case TYPE_CHAR:
			return typeInteger;
		case TYPE_POINTER:
			return compatible_arithmetic_OP(type1, type2->refType);
		default:
			yyerror("Type mismatch. Invalid operands for arithmetic operation ");
			break;
		}
		break;
	case TYPE_REAL:
		switch (type2->kind) {
		case TYPE_REAL:
			return typeReal;
		case TYPE_INTEGER:
			return typeInteger;
		case TYPE_BOOLEAN:
			yyerror("Type mismatch. Invalid operands for arithmetic operation ");
			break;
		case TYPE_CHAR:
			return typeInteger;
		case TYPE_POINTER:
			return compatible_arithmetic_OP(type1, type2->refType);
		default:
			yyerror("Type mismatch. Invalid operands for arithmetic operation ");
			break;
		}
		break;
	case TYPE_BOOLEAN:
		yyerror("Type mismatch. Invalid operands for arithmetic operation ");
		break;
	case TYPE_CHAR:
		switch (type2->kind) {
		case TYPE_REAL:
			return typeReal;
		case TYPE_INTEGER:
			return typeInteger;
		case TYPE_BOOLEAN:
			yyerror("Type mismatch. Invalid operands for arithmetic operation ");
			break;
		case TYPE_CHAR:
			return typeInteger;
		case TYPE_POINTER:
			return compatible_arithmetic_OP(type1, type2->refType);
		default:
			yyerror("Type mismatch. Invalid operands for arithmetic operation ");
			break;
		}
		break;
	case TYPE_POINTER:
		return compatible_arithmetic_OP(type1->refType, type2);
	default:
		yyerror("Type mismatch. Invalid operands for arithmetic operation ");
		break;
	}
	return NULL;
	}



/***************	END COMPATIBILITY FUNCTIONS		******************/


/***************	STACK IMPLEMENTATION			******************/
/*SOURCE : http://www.c4learn.com/c-programs/c-program-to-implement-stack-operations-using-singly-linked-list.html*/


typedef struct stack {
	SymbolEntry *data;
	Type dataL;
	SymbolEntry *dataSE;
	int flag;
	struct stack *next;
} node;


void Push(SymbolEntry *, node **);
void Display(node **);
SymbolEntry *Pop(node **);
int Sempty(node *);



node *top;

void Push(SymbolEntry *Item, node **top)
{
	node *New;

	node *get_node(SymbolEntry *);
	New = get_node(Item);
	New->next = *top;
	*top = New;
}

node *get_node(SymbolEntry *item)
{
	node *temp;

	temp = (node *) malloc(sizeof(node));
	if (temp == NULL)
		fprintf(stdout, "\nMemory Cannot be allocated");
	temp->data = item;
	temp->next = NULL;
	return temp;
}

int Sempty(node *temp)
{
	if (temp == NULL)
		return 1;
	else
		return 0;
}

SymbolEntry *Pop(node **top)
{
	SymbolEntry *item;
	node *temp;

	item = (*top)->data;
	temp = *top;
	*top = (*top)->next;
	free(temp);
	return item;
}

void Display(node **head)
{
	node *temp;

	temp = *head;
	if (Sempty(temp))
		fprintf(stdout, "\nThe stack is empty!");
	else {
		while (temp != NULL) {
		 //fprintf(stdout, "%d\n", temp->data);
		 temp = temp->next;
		}
	}
	//getch();
}
/***************	END STACK IMPLEMENTATION			******************/

/***************	ANOTHER STACK IMPLEMENTATION			******************/

void PushL(Type, node **);
void DisplayL(node **);
Type PopL(node **);
int SemptyL(node *);

node *l_value_stack;

void PushL(Type Item, node **top)
{
	node *New;

	node *get_nodeL(Type);
	New = get_nodeL(Item);
	New->next = *top;
	*top = New;
}

node *get_nodeL(Type item)
{
	node *temp;

	temp = (node *) malloc(sizeof(node));
	if (temp == NULL)
		fprintf(stdout, "\nMemory Cannot be allocated");
	temp->dataL = item;
	temp->next = NULL;
	return temp;
}

int SemptyL(node *temp)
{
	if (temp == NULL)
		return 1;
	else
		return 0;
}

Type PopL(node **top)
{
	Type item;
	node *temp;

	item = (*top)->dataL;
	temp = *top;
	*top = (*top)->next;
	free(temp);
	return item;
}

void DisplayL(node **head)
{
	node *temp;

	temp = *head;
	if (SemptyL(temp))
		fprintf(stdout, "\nThe stack is empty!");
	else {
		while (temp != NULL) {
		 //fprintf(stdout, "%d\n", temp->data);
		 temp = temp->next;
		}
	}
	//getch();
}
/***************	END						******************/



/***************	ANOTHER STACK IMPLEMENTATION			******************/

void PushSE(SymbolEntry *, node **);
void DisplaySE(node **);
SymbolEntry *PopSE(node **);
int SemptySE(node *);

node *se_stack;
node *call_result_stack;

void PushSE(SymbolEntry *Item, node **top)
{
	node *New;

	node *get_nodeSE(SymbolEntry *);
	New = get_nodeSE(Item);
	New->next = *top;
	*top = New;
}

node *get_nodeSE(SymbolEntry *item)
{
	node *temp;

	temp = (node *) malloc(sizeof(node));
	if (temp == NULL)
		fprintf(stdout, "\nMemory Cannot be allocated");
	temp->dataSE = item;
	temp->next = NULL;
	return temp;
}

int SemptySE(node *temp)
{
	if (temp == NULL)
		return 1;
	else
		return 0;
}

SymbolEntry *PopSE(node **top)
{
	SymbolEntry *item;
	node *temp;

	item = (*top)->dataSE;
	temp = *top;
	*top = (*top)->next;
	free(temp);
	return item;
}

void DisplaySE(node **head)
{
	node *temp;

	temp = *head;
	if (SemptySE(temp))
		fprintf(stdout, "\nThe stack is empty!");
	else {
		while (temp != NULL) {
		 //fprintf(stdout, "%d\n", temp->data);
		 temp = temp->next;
		}
	}
}
/***************	END						******************/



/***************	ANOTHER STACK IMPLEMENTATION			******************/

void PushArray(int , node **);
int PopArray(node **);

node *array_stack;

void PushArray(int item, node **top)
{
	node *New;

	node *get_nodeArray(int);
	New = get_nodeArray(item);
	New->next = *top;
	*top = New;
}

node *get_nodeArray(int item)
{
	node *temp;
	temp = (node *) malloc(sizeof(node));
	if (temp == NULL)
		fprintf(stdout, "\nMemory Cannot be allocated");
	temp->flag = item;
	temp->next = NULL;
	return temp;
}

int PopArray(node **top)
{
	int item;
	node *temp;

	item = (*top)->flag;
	temp = *top;
	*top = (*top)->next;
	free(temp);
	return item;
}

/***************	END						******************/

