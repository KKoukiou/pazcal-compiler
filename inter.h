#include "symbol.h"
#include "general.h"


/*είδος λειτουργείας τετράδας*/
typedef enum {
	OP_UNIT,
	OP_ENDU,
	OP_JUMP,
    OP_JUMPL,
	OP_LABEL,
	OP_IFB,
	OP_CALL,
	OP_PAR,
	OP_RET,
	OP_RETV,
    OP_ARRAY,
	OP_eq,
	OP_neq,
	OP_less,
	OP_greater,
	OP_leq,
	OP_geq,	
	OP_PLUS,
	OP_MINUS,
	OP_bmul,
	OP_bdiv,
	OP_bmod,
	OP_mod,
	OP_assign
}oper;

typedef enum{
	QUAD_EMPTY,
	QUAD_INTEGER,	/*	integer constant			*/
	QUAD_REAL,		/*	real constant				*/
	QUAD_CHAR, 		/*	char constant				*/
	QUAD_BOOL,		/*	bool constant				*/
	QUAD_STR,		/*	char array constant			*/
	QUAD_SE,		/*	variable of any kind 		*/
	QUAD_MODE,		/*	pass by value or reference	*/
	QUAD_TOFILL,	/*	fill later the labels quad	*/
	QUAD_TAG,		/*	label						*/
	QUAD_POINTER 	/*	pointer						*/
} QUAD_TYPE;

typedef struct quad{
	QUAD_TYPE type;
	PassMode mode;
	union{
		int intval;
		long double floatval;
		char *strval;
		SymbolEntry *se;
	}value;
}quad;

typedef struct quad_list {
	int n ;
	oper op;
	quad *x;
	quad *y;
	quad *z;
	struct quad_list *next;
}qnode;

/*είδος to σε for expression */
typedef enum{
	_TO,
	_DOWN_TO
} TO_TYPE;

/*δείκτες για την καθολική λίστα αποθήκευσης του κώδικα τετράδων*/
qnode *head_quad;
qnode *current_quad;

/*βοηθητικές συναρτήσεις για επεξεργασία της λίστας τετράδων*/
void Insert(oper op, quad *x, quad *y, quad *z,qnode **);
void DisplayQuads(qnode **);
void printQ(quad *q);
void printOP(oper op);

int NEXTQUAD();
void GENQUAD(oper op, quad *x,quad *y,quad *z);
int NEXTTEMP(Type t);
void BACKPATCH(qnode ** head, quad *q);

/*λίστα από τελούμενα z τετράδων και ετικέτες αυτών ώστε να πετύχουμε 
BACKPATCHING σε Ο(ν) για ν μέγεθος λίστας */
typedef struct condition_list {
	quad *n;
	int tag;
	struct condition_list *next;
} condition;

void _BACKPATCH(condition ** head, int q);
condition *MERGE(condition ** l1, condition ** l2 );
condition *MAKELIST(quad *tag, int quadnumber);
condition *EMPTYLIST();
qnode *get_qnode(oper op , quad *x, quad *y , quad *z );
 
void InsertC(quad *tag,condition **, condition **, int quadnumber);
void DisplayCList(condition **);



typedef struct stackNEXT {
        condition *data;
        int while_backQUAD;
	struct stackNEXT *next;
} node_N;

void Push_N(condition *, node_N **);
condition *Pop_N(node_N **);
int Sempty_N(node_N *);

void Push_W(int, node_N **);
int Pop_W(node_N **);
void DisplayWHILE(node_N **);


typedef struct stackFORLOOP {
        SymbolEntry *for_counter;
        int for_backQUAD;
		SymbolEntry *for_se;
	struct stackFORLOOP *next;
} node_F;


void Push_F(int ,SymbolEntry *, SymbolEntry *, node_F **);
node_F Pop_F(node_F **);

/*Maintype*/
struct V{
	condition *headTRUE;
	condition *headFALSE;
	SymbolEntry *se;
	Type type;
	bool calculated ;
	union{
		char *strvalue;
		int value;
		long double floatval;
	};
};
typedef struct V Vinfo;

/*συνάρτηση δημιουργίας τετράδων για τις πράξεις = > < >= <= != */
void intercode_relop(Vinfo *dd, Vinfo *d1, Vinfo *d3, oper op);

/*συνάρτηση δημιουργίας τετράδων για τις αριθμητικές πράξεις + - / *  mod */
void intercode_arithmetic_op(Vinfo *dd, Vinfo *d1, Vinfo *d3, oper op);

/*συνάρτηση δημιουργίας τετράδων για τις πράξεις ανάθεσης*/
void intercode_assign_op(Vinfo *d1, Vinfo *d3);

/*συνάρτηση δημιουργίας τετράδων για τις πράξεις ανάθεσης*/
void intercode_complex_assign_op(Vinfo *d1, Vinfo *d3, oper op);

/*συνάρτηση δημιουργίας τετράδων για τις παραμέτρους συναρτήσεων*/
void intercode_PAR_op(SymbolEntry ** current, Vinfo *d1);

/*συνάρτηση μετατροπης από condition σε expression*/
SymbolEntry *conversion_from_condition_to_expression(Vinfo *d);

/*συνάρτηση μετατροπης από expression σε condition*/
void conversion_from_expression_to_condition(Vinfo *d0, Vinfo *d1);

/*συνάρτηση δημιουργίας τετράδων με δοσμένο όρισμα επιστροφής (για το FOR) */
void intercode_arithmetic_op_givenRET(Vinfo *dd, Vinfo *d1, Vinfo *d3, oper op);

void initialize_array_to_zero(SymbolEntry *se);
