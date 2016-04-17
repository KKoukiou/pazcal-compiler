%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inter.h"
#include "symbol.h"
#include "routines.h"

void yyerror (const char *msg);
int yylex();

PassMode formal_mode;

Type const_expr_type;
Type var_type;
Type functype;

char *formal_name;

SymbolEntry *current;
SymbolEntry *se;
SymbolEntry *f;
SymbolEntry *f_se;
SymbolEntry *program;
SymbolEntry *call_res;
condition *condFALSE ;
condition *L1;
condition *L2;

node_N *head_L1;
node_N *head_L2;
node_N *head_condFALSE;
node_N *while_stack;

int while_backQUAD;
int for_backQUAD;
node_F *for_stack;
condition *conditionFALSE;
SymbolEntry *for_counter;

/*
is 1 if we are inside a routine
*/
bool is_global = 1;
int array;
condition *for_list;


#include "aux.c"

%}

%union {
	char *n;
	int num;
	long double real;
	Type t;
	Vinfo v;
	condition *headNEXT;
	TO_TYPE to;
};

%token T_id			"id"
%token T_for 		"FOR"
%token T_next		"NEXT"
%token T_return		"return"
%token T_write		"WRITE"
%token T_bool		"bool"
%token T_default	"default"
%token T_form		"FORM"
%token T_step		"STEP"
%token T_writeln	"WRITELN"
%token T_break		"break"
%token T_do			"do"
%token T_func		"FUNC"
%token T_switch		"switch"
%token T_writesp	"WRITESP"
%token T_case 		"case"
%token T_downto		"DOWNTO"
%token T_if			"if"
%token T_proc		"PROC"
%token T_to			"TO"
%token T_writespln	"WRITESPLN"
%token T_char		"char"
%token T_else		"else"
%token T_int		"int"
%token T_program	"PROGRAM"
%token T_true		"true"
%token T_const		"const"
%token T_false		"false"
%token T_real		"REAL"
%token T_while		"while"
%token T_continue 	"continue"
%token T_mod 		"MOD"
%token T_int_const		"int_const"
%token T_float_const	"float_const"
%token T_char_const		"char_const"
%token T_string_const	"string_const"
%token T_pe			"+="
%token T_me			"-="
%token T_mule		"*="
%token T_dive		"/="
%token T_mode		"%="
%token T_plusplus 	"++"
%token T_minusminus	"--"


%left T_or T_bor
%left T_band T_and
%left T_eq T_neq
%left T_less T_greater T_leq T_geq
%left '+' '-'
%left T_bmul  T_bdiv T_bmod T_mod
%left T_uminus T_uplus T_unot T_not


%nonassoc "then"
%nonassoc "else"

%type<real> 	"float_const"
%type<num> 		"int_const"
%type<num> 		"char_const"
%type<n> 		"string_const"
%type<n> 		"id"
%type<t> 		type
%type<v> 		expr
%type<v> 		const_expr
%type<v> 		l_value
%type<v> 		more_expr_br
%type<headNEXT>	stmt
%type<headNEXT> thenclause
%type<headNEXT> block
%type<headNEXT> inner_block
%type<t> 		call
%type<v> 		step
%type<to> 		to
%type<t> 		matrixD
%type<headNEXT> local_def
%%

/*	Recursion: 'right is wrong'	*/

module		:/*empty*/
		|module declaration
		;
declaration	:
		const_def
		|var_def
		|routine
		|program
		;
const_expr_more :/*empty*/
		|const_expr_more ',' "id" '=' const_expr
		{
			compatible_assignment(const_expr_type, $5.type);
			switch (const_expr_type->kind) {
			case TYPE_REAL:
				if($5.type->kind == TYPE_REAL)
					se = newConstant($3, const_expr_type, $5.floatval);
				else if ($5.type->kind == TYPE_INTEGER)
					se = newConstant($3, const_expr_type, $5.value);
				break;
			case TYPE_INTEGER:
				se = newConstant($3, const_expr_type, $5.value);
				break;
			case TYPE_BOOLEAN:
				se = newConstant($3, const_expr_type, $5.value);
				break;
			case TYPE_CHAR:
				if ($5.type->kind == TYPE_INTEGER) 
					se = newConstant($3, const_expr_type, $5.value & 0xFF);
				else if ($5.type->kind == TYPE_CHAR) 
					se = newConstant($3, const_expr_type, $5.value);
				break;
			default:
				printf("Only basic types can be constant expressions\n");
			}
		}
		;

const_def 	:"const" type {
				const_expr_type = $2;
			}"id" '=' const_expr const_expr_more ';'
			{
			compatible_assignment($2, $6.type);
			switch (const_expr_type->kind) {
			case TYPE_REAL:
				if ($6.type->kind == TYPE_REAL)
					se = newConstant($4, const_expr_type, $6.floatval);
				else if ($6.type->kind == TYPE_INTEGER)
					se = newConstant($4, const_expr_type, $6.value);
				break;
			case TYPE_INTEGER:
				se = newConstant($4, const_expr_type, $6.value);
				break;
			case TYPE_BOOLEAN:
				se = newConstant($4, const_expr_type, $6.value);
				break;
			case TYPE_CHAR:
				if ($6.type->kind == TYPE_INTEGER)
					se = newConstant($4, const_expr_type, $6.value & 0xFF);
				else if ($6.type->kind == TYPE_CHAR)
					se = newConstant($4, const_expr_type, $6.value);
				break;
			default:
				printf("Only basic types can be constant expressions\n");
			}
		}
		;

var_def :type
		{
			var_type = $1;
		}
		var_init ';'
		;

var_init	:"id" '=' expr
		{
			se = newVariable($1, var_type);
			if (is_global)
				if($3.calculated != 1)
					yyerror("Global variable if initialized must be with an "
							"evaluated expression");
			/*Quadruples code*/
			Vinfo a;
			a.type = var_type;
			a.calculated = 0;
			a.se = se;
			intercode_assign_op(&a, &$3);
			/*End Quadruples code*/
		}
		|"id"
		{
			se = newVariable($1, var_type);
			if (is_global) {
				/*If global and not initialized, initialize to zero*/
				Vinfo a ;
				a.type = var_type;
				a.se = se;
				a.calculated = 0;
				Vinfo zero;
				zero.type = typeInteger;
				zero.calculated = 1;
				zero.value = 0;
				intercode_assign_op(&a, &zero);
			}
		}
		|"id" '[' const_expr ']'
		{
			if (!$3.calculated)
				yyerror("IndexError:At declaration Array index must be a "
						"constant value");
			else if ($3.calculated && ($3.value <= 0))
				yyerror("IndexError:Array index negative number");
		}
		matrixD
		{
			newVariable($1, typeArray($3.value, $6));
		}
		|var_init ',' "id" '=' expr
		{
			se = newVariable($3, var_type);

			if (is_global && $5.calculated != 1)
					yyerror("Global variable if initialized must be with an "
							"evaluated expression");

			/*Quadruples code*/
			Vinfo a;
			a.type = var_type;
			a.se = se;
			a.calculated = 0;
			intercode_assign_op(&a ,&$5);
			/*End Quadruples code*/


		}
		|var_init ',' "id"
		{
			se = newVariable($3, var_type);

			if (is_global) {
				Vinfo a ;
				a.type = var_type;
				a.calculated = 0;
				a.se = se;
				Vinfo zero;
				zero.type = typeInteger;
				zero.calculated = 1;
				zero.value = 0;
				intercode_assign_op(&a, &zero);
			}
		}
		|var_init ',' "id" '[' const_expr ']'
		{
			if (!$5.calculated && ($5.type != typeInteger))
				yyerror("IndexError:At declaration Array index must be a "
						"constant value");
			if ($5.calculated && ($5.value < 0))
				yyerror("IndexError:Array index out of range");

			if (is_global) {
				/*must initialize everything to zero*/
			}
		}
		matrixD
		{
			newVariable($3, typeArray($5.value, $8));
		}
		;

matrixD 	:/*empty*/
		{
			$$ = var_type;
		}
		|'[' const_expr ']'
		{
			if (!$2.calculated)
				yyerror("IndexError: Array index must be a constant value");
			if ($2.calculated && (!$2.value > 0))
				yyerror("IndexError: Array index out of range");
		}
		matrixD 	/*right recursion  :(*/
		{
			$$ = typeArray($2.value, $5);
		}
		;
more_arguments 	: /*empty*/
		|
		',' type
		{
			var_type = $2;
		} formal
		{
			se = newParameter(formal_name, var_type, formal_mode, f);
		}
		more_arguments  	//right recursion :( :(
		;
arguments	:/*empty*/
		|
		type
		{
			var_type = $1;
		} formal
		{
			se = newParameter(formal_name, var_type, formal_mode, f);
		}
		more_arguments
		;

formal	:"id"
		{
	 		formal_name = $1;
			formal_mode = PASS_BY_VALUE;
		}
		|'&' "id"
		{
			formal_name = $2;
			formal_mode = PASS_BY_REFERENCE;
		}
		|"id" '[' const_expr ']' matrixD
		{
			if ($3.calculated && ($3.value <= 0))
				yyerror("IndexError: Array index out of range");
			formal_name = $1;
			formal_mode = PASS_BY_REFERENCE;
			var_type = typeArray($3.value, $5);
		}
		|"id" '[' ']' matrixD
		 /*In parameters we can omit the first dimension of a matrix*/
		{
			formal_name = $1;
			formal_mode = PASS_BY_REFERENCE;
			var_type = typeIArray($4);
		}
		;
proc 	:"PROC" "id" '('
		{
			f = newFunction($2);
			if (f == NULL) 
				yyerror("Dublicate identifier");
			openScope();
		
			/*Thats needed for recursive function bodies if no definition is
			 * ahead*/
			f->u.eFunction.resultType = typeVoid;

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = f;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_UNIT, x, y, z);
			/*End Quadruples code*/
		}
		;
func		:"FUNC" type "id" '('
		{
			functype = $2;
			f = newFunction($3);
			if (f == NULL)
				yyerror("Dublicate identifier");
			openScope();

			/*Thats needed for recursive function bodies if no definition is
			 * ahead*/
			f->u.eFunction.resultType = functype;

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = f;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_UNIT, x, y, z);
			/*End Quadruples code*/
		}
		;
routine 	:proc arguments ')' ';'
		{
			closeScope();
			forwardFunction(f);
			endFunctionHeader(f, typeVoid);

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = f;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_ENDU, x, y, z);
			/*End Quadruples code*/
		}
		|func arguments ')' ';'
		{
			closeScope();
			forwardFunction(f);
			endFunctionHeader(f, functype);

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = f;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_ENDU, x, y, z);
			/*End Quadruples code*/
		}
		|proc arguments ')'
		{
			is_global = 0;
		}
		block
		{
			is_global = 1;
			closeScope();
			endFunctionHeader(f, typeVoid);

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = f;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_ENDU, x, y, z);
			/*End Quadruples code*/
		}
		|func arguments ')'
		{
			is_global = 0;
		}
		block
		{
			is_global = 1;

			closeScope();
			endFunctionHeader(f, functype);

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = f;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_ENDU, x, y, z);
			/*End Quadruples code*/

		}
		;
program_header	:"PROGRAM" "id" '(' ')'
		{
			program = newFunction($2);
			openScope();

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = program;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_UNIT, x, y, z);
			/*End Quadruples code*/
		}
		;
program 	:program_header
		{
			is_global = 0;
		}
		block
		{
			is_global = 1;
			closeScope();
			endFunctionHeader(program, typeVoid);

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			x->value.se = program;
			x->type = QUAD_SE;
			quad *y = (quad *) new(sizeof(quad));
			y->type = QUAD_EMPTY;
			quad *z = (quad *) new(sizeof(quad));
			z->type = QUAD_EMPTY;
			GENQUAD(OP_ENDU, x, y, z);
			/*End Quadruples code*/

		}
		;
type	:"int"		{ $$ = typeInteger;	}
		|"bool"		{ $$ = typeBoolean;	}
		|"char" 	{ $$ = typeChar;	}
		|"REAL"		{ $$ = typeReal;	}
		;

const_expr	:expr
		{
			$$ = $1;
			if ($1.calculated ==0) {
				yyerror("Only constant expressions or can be assigned"
						" to constant expressions");
			}
		}
		;
expr 	:"int_const"
		{
			$$.type = typeInteger;
			$$.value = $1;
			$$.calculated = 1;
		}
		|"float_const"
		{
			$$.type = typeReal;
			$$.floatval = $1;
			$$.calculated = 1;
			printf("Read Value is %Lf\n",$$.floatval );
		}
		|"string_const"
		{
			$$.type = typeArray(strlen($1) + 1, typeChar);
			$$.strvalue = strdup($1);
			$$.calculated = 1;
		}
		|"char_const"
		{
			$$.type = typeChar;
			$$.value = $1;
			$$.calculated = 1;
		}
		|"true"
		{
			$$.type = typeBoolean;
			$$.value = 1;
			$$.calculated = 1;
			$$.headTRUE = EMPTYLIST();
			$$.headFALSE = EMPTYLIST();
		}
		|"false"
		{
			$$.type = typeBoolean;
			$$.value = 0;
			$$.calculated = 1;
			$$.headTRUE = EMPTYLIST();
			$$.headFALSE = EMPTYLIST();
		}
		|'(' expr ')'
		{
			$$ = $2;
		}
		|l_value
		{
			if ($1.se->entryType == ENTRY_CONSTANT)
				$$.calculated = 1;
			else 
				$$.calculated = 0;
			$$.type = $1.type;
			if ($1.se->entryType == ENTRY_CONSTANT) {
				switch ($1.type->kind) {
				case TYPE_REAL:
					$$.floatval = $1.se->u.eConstant.value.vReal;
					break;
				case TYPE_INTEGER:
					$$.value = $1.se->u.eConstant.value.vInteger;
					break;
				case TYPE_BOOLEAN:
					$$.value = $1.se->u.eConstant.value.vBoolean;
					break;
				case TYPE_CHAR:
					$$.value = $1.se->u.eConstant.value.vChar;
					break;
				default:
					printf("Unknown expression type\n");
				}
			}
			else 
				$$.se = $1.se;
		}
		|call
		{
			$$.calculated = 0;
			$$.se = call_res;
			$$.type = $1;

			/*Quadruples code */
			if ($1 == typeBoolean) {
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));

				x->type = QUAD_SE;
				x->value.se = call_res;
				y->type = QUAD_EMPTY;
				z->type = QUAD_TOFILL;

				$$.headTRUE = MAKELIST(z,quadNext);
				GENQUAD(OP_IFB, x, y, z);

				x = (quad *) new(sizeof(quad));
				y = (quad *) new(sizeof(quad));
				z = (quad *) new(sizeof(quad));

				x->type = QUAD_EMPTY;
				y->type = QUAD_EMPTY;
				z->type = QUAD_TOFILL;

				$$.headFALSE = MAKELIST(z, quadNext);
				GENQUAD(OP_JUMP, x, y, z);
			}
			/*End Quadruples code */
			call_res = PopSE(&call_result_stack);
		}
		| '+' expr
		{
			$$.type = $2.type;
			$$.calculated = $2.calculated;

			if ($$.calculated) 
				if ($$.type == typeReal)
					$$.floatval = $2.floatval;
				else 
					$$.value = $2.value;
			else {
				/*Quadruples code*/
				Vinfo a;
				a.type = typeInteger;
				a.value = 0;
				a.calculated = 1;
				intercode_arithmetic_op(&$$, &a, &$2, OP_PLUS);
				/*End Quadruples code*/
			}
		}	%prec T_uplus
		|'-' expr
		{
			$$.type = $2.type;
			$$.calculated = $2.calculated;
			if ($$.calculated) { 
				if (equalType($$.type, typeReal))
					$$.floatval = -$2.floatval;
				else 
					$$.value = -$2.value;
			}
			else {
				/*Quadruples code*/
				Vinfo a;
				a.type = typeInteger;
				a.value = 0;
				a.calculated = 1;
				intercode_arithmetic_op(&$$, &a, &$2, OP_MINUS);
				/*End Quadruples code*/
			}
		} 	%prec T_uminus
		|expr '+' expr
		{
			compatible_arithmetic_OP($1.type, $3.type);
			$$.calculated = $1.calculated && $3.calculated;

			if ($$.calculated) {
				switch ($1.type->kind) {
				case TYPE_REAL:
					$$.type = typeReal;
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.floatval = $1.floatval + $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.floatval = $1.floatval + $3.value;
						break;
					case TYPE_CHAR:
						$$.floatval = $1.floatval + $3.value;
						break;
					default:
						yyerror("Type mismatch '+'");
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.floatval = $1.value + $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value + $3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value + $3.value;
						break;
					default:
						yyerror("Type mismatch '+'");
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.value = $1.value + $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value + $3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value + $3.value;
						break;
					default:
						yyerror("Type mismatch '+'");
						break;
					}
					break;
				default:
					yyerror("Type mismatch '+'");
					break;
				}
			}
			/*Start Quadruples code*/
			if (!$$.calculated)
				intercode_arithmetic_op(&$$, &$1, &$3, OP_PLUS);
			/*End Quadruples code*/
		}
		|expr '-' expr
		{
			compatible_arithmetic_OP($1.type, $3.type);

			$$.calculated = $1.calculated && $3.calculated;
			if ($$.calculated) {
				switch ($1.type->kind) {
				case TYPE_REAL:
					$$.type = typeReal;
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.floatval = $1.floatval - $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.floatval = $1.floatval - $3.value;
						break;
					case TYPE_CHAR:
						$$.floatval = $1.floatval - $3.value;
						break;
					default:
						yyerror("Type mismatch '-'");
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.floatval = $1.value - $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value - $3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value - $3.value;
						break;
					default:
						yyerror("Type mismatch '-'");
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.value = $1.value - $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value - $3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value - $3.value;
						break;
					default:
						yyerror("Type mismatch '-'");
						break;
					}
					break;
				default:
					yyerror("Type mismatch '-'");
					break;
				}
			}


			/*Start Quadruples code*/
			if (!$$.calculated)
			intercode_arithmetic_op(&$$, &$1, &$3, OP_MINUS);
			/*End Quadruples code*/
		}
		|expr  T_bmul expr
		{
			compatible_arithmetic_OP($1.type, $3.type);
			$$.calculated = $1.calculated && $3.calculated;
			if ($$.calculated) {
				switch ($1.type->kind) {
				case TYPE_REAL:
					$$.type = typeReal;
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.floatval = $1.floatval *$3.floatval;
						break;
					case TYPE_INTEGER:
						$$.floatval = $1.floatval *$3.value;
						break;
					case TYPE_CHAR:
						$$.floatval = $1.floatval *$3.value;
						break;
					default:
						yyerror("Type mismatch '*'");
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.floatval = $1.value *$3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value *$3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value *$3.value;
						break;
					default:
						yyerror("Type mismatch '*'");
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.value = $1.value *$3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value *$3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value *$3.value;
						break;
					default:
						yyerror("Type mismatch '*'");
						break;
					}
					break;
				default:
					yyerror("Type mismatch '*'");
					break;
				}
			}


			/*Start Quadruples code*/
			if (!$$.calculated)
			intercode_arithmetic_op(&$$, &$1, &$3, OP_bmul);
			/*End Quadruples code*/
		}
		|expr T_bdiv expr
		{
			if ($3.calculated == 1 &&
				(($3.value == 0 &&
				($3.type == typeInteger || $3.type == typeChar)) ||
				($3.floatval == 0 && $3.type == typeReal)))
				yyerror("ZeroDivisionError: integer division or modulo by zero.");

			compatible_arithmetic_OP($1.type, $3.type);
			$$.calculated = $1.calculated && $3.calculated;
			if ($$.calculated) {
				switch ($1.type->kind) {
				case TYPE_REAL:
					$$.type = typeReal;
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.floatval = $1.floatval / $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.floatval = $1.floatval / $3.value;
						break;
					case TYPE_CHAR:
						$$.floatval = $1.floatval / $3.value;
						break;
					default:
						yyerror("Type mismatch '/'");
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.floatval = $1.value / $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value / $3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value / $3.value;
						break;
					default:
						yyerror("Type mismatch '/'");
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.type = typeReal;
						$$.value = $1.value / $3.floatval;
						break;
					case TYPE_INTEGER:
						$$.type = typeInteger;
						$$.value = $1.value / $3.value;
						break;
					case TYPE_CHAR:
						$$.type = typeInteger;
						$$.value = $1.value / $3.value;
						break;
					default:
						yyerror("Type mismatch '/'");
						break;
					}
					break;
				default:
					yyerror("Type mismatch '/'");
					break;
				}
			}

			/*Start Quadruples code*/
			if (!$$.calculated)
			intercode_arithmetic_op(&$$, &$1, &$3, OP_bdiv);
			/*End Quadruples code*/
		}
		|expr T_bmod expr
		{
			$$.calculated=0;
			if ($3.calculated == 1 && $3.value == 0 &&
				($3.type == typeInteger || $3.type == typeChar))
				yyerror("ZeroDivisionError: integer division or modulo by zero"
						".");
			else if (($3.type == typeInteger || $3.type == typeChar) &&
					($1.type == typeInteger || $3.type == typeChar)) {
				$$.type =typeInteger;
				if ($1.calculated && $3.calculated) {
					$$.calculated = 1;
					$$.value = ($1.value % $3.value);
				}
			}
			else yyerror("Type mismatch. expression must be of type Integer or"
						 " Char ");

			/*Start Quadruples code*/
			if (!$$.calculated)
				intercode_arithmetic_op(&$$, &$1, &$3, OP_bmod);
			/*End Quadruples code*/
		}
		|expr "MOD" expr
		{
			$$.calculated=0;
			if ($3.calculated==1 && $3.value == 0 &&
				($3.type == typeInteger || $3.type == typeChar))
				yyerror("ZeroDivisionError: integer division or modulo by zero"
						".");
			else if (($3.type == typeInteger || $3.type == typeChar) &&
					($1.type == typeInteger || $3.type == typeChar)) {
				$$.type = typeInteger;
				if ($1.calculated && $3.calculated) {
					$$.calculated = 1;
					$$.value = ($1.value % $3.value);
				}
			}
			else yyerror("Type mismatch. expression must be of type Integer or"
						" Char");

			/*Start Quadruples code*/
			if ( !$$.calculated)
				intercode_arithmetic_op(&$$, &$1, &$3, OP_bmod);
			/*End Quadruples code*/
		}
		//Start relop
		|expr T_eq expr
		{
			compatible_arithmetic_OP($1.type, $3.type);

			$$.type = typeBoolean;
			$$.calculated = 0;

			if (($1.calculated) && ($3.calculated)) {
				$$.calculated = 1;
				switch ($1.type->kind) {
				case TYPE_REAL:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.floatval == $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.floatval == $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.floatval == $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value == $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value == $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value == $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value == $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value == $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value == $3.value);
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			} else {
				/*Quadruples Code*/
				intercode_relop(&$$, &$1, &$3, OP_eq);
				/*End Quadruples Code*/
			}
		}
		|expr  T_neq expr
		{
			compatible_arithmetic_OP($1.type, $3.type);

			$$.type = typeBoolean;
			$$.calculated = 0;

			if (($1.calculated) && ($3.calculated)) {
				$$.calculated = 1;
				switch ($1.type->kind) {
				case TYPE_REAL:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.floatval != $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.floatval != $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.floatval != $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value != $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value != $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value !=  $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value != $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value != $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value !=  $3.value);
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			} else {
				/*Quadruples Code*/
				intercode_relop(&$$, &$1, &$3, OP_neq);
				/*End Quadruples Code*/
			}
		}
		|expr T_greater expr
		{
			compatible_arithmetic_OP($1.type, $3.type);

			$$.type = typeBoolean;
			$$.calculated = 0;

			if (($1.calculated) && ($3.calculated)) {
				$$.calculated = 1;
				switch ($1.type->kind) {
				case TYPE_REAL:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.floatval > $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.floatval > $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.floatval > $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value > $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value > $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value > $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value > $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value > $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value > $3.value);
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			} else {
				/*Quadruples Code*/
				intercode_relop(&$$, &$1, &$3, OP_greater);
				/*End Quadruples Code*/
			}
		}
		|expr T_less expr
		{
			compatible_arithmetic_OP($1.type, $3.type);

			$$.type = typeBoolean;
			$$.calculated = 0;

			if (($1.calculated) && ($3.calculated)) {
				$$.calculated = 1;
				switch ($1.type->kind) {
				case TYPE_REAL:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.floatval < $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.floatval < $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.floatval < $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_INTEGER:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value < $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value < $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value < $3.value);
						break;
					default:
						break;
					}
					break;
				case TYPE_CHAR:
					switch ($3.type->kind) {
					case TYPE_REAL:
						$$.value = ($1.value < $3.floatval);
						break;
					case TYPE_INTEGER:
						$$.value = ($1.value < $3.value);
						break;
					case TYPE_CHAR:
						$$.value = ($1.value < $3.value);
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			} else {
				/*Quadruples Code*/
				intercode_relop(&$$, &$1, &$3, OP_less);
				/*End Quadruples Code*/
			}
		}
		|expr T_leq expr {
			compatible_arithmetic_OP($1.type, $3.type);

			$$.type = typeBoolean;
			$$.calculated = 0;

			if (($1.calculated) && ($3.calculated)) {
			$$.calculated = 1;
			switch ($1.type->kind) {
			case TYPE_REAL:
				switch ($3.type->kind) {
				case TYPE_REAL:
					$$.value = ($1.floatval <= $3.floatval);
					break;
				case TYPE_INTEGER:
					$$.value = ($1.floatval <= $3.value);
					break;
				case TYPE_CHAR:
					$$.value = ($1.floatval <= $3.value);
					break;
				default:
					break;
				}
				break;
			case TYPE_INTEGER:
				switch ($3.type->kind) {
				case TYPE_REAL:
					$$.value = ($1.value <= $3.floatval);
					break;
				case TYPE_INTEGER:
					$$.value = ($1.value <= $3.value);
					break;
				case TYPE_CHAR:
					$$.value = ($1.value <=  $3.value);
					break;
				default:
					break;
				}
				break;
			case TYPE_CHAR:
				switch ($3.type->kind) {
				case TYPE_REAL:
					$$.value = ($1.value <= $3.floatval);
					break;
				case TYPE_INTEGER:
					$$.value = ($1.value <= $3.value);
					break;
				case TYPE_CHAR:
					$$.value = ($1.value <= $3.value);
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			} else {
			/*Quadruples Code*/
			intercode_relop(&$$, &$1, &$3, OP_leq);
			/*End Quadruples Code*/
			}
		}
		|expr  T_geq expr
		{
			compatible_arithmetic_OP($1.type, $3.type);

			$$.type = typeBoolean;
			$$.calculated = 0;

			if (($1.calculated) && ($3.calculated)) {
			$$.calculated = 1;
			switch ($1.type->kind) {
			case TYPE_REAL:
				switch ($3.type->kind) {
				case TYPE_REAL:
					$$.value = ($1.floatval >= $3.floatval);
					break;
				case TYPE_INTEGER:
					$$.value = ($1.floatval >= $3.value);
					break;
				case TYPE_CHAR:
					$$.value = ($1.floatval >= $3.value);
					break;
				default:
					break;
				}
				break;
			case TYPE_INTEGER:
				switch ($3.type->kind) {
				case TYPE_REAL:
					$$.value = ($1.value >= $3.floatval);
					break;
				case TYPE_INTEGER:
					$$.value = ($1.value >= $3.value);
					break;
				case TYPE_CHAR:
					$$.value = ($1.value >= $3.value);
					break;
				default:
					break;
				}
				break;
			case TYPE_CHAR:
				switch ($3.type->kind) {
				case TYPE_REAL:
					$$.value = ($1.value >= $3.floatval);
					break;
				case TYPE_INTEGER:
					$$.value = ($1.value >= $3.value);
					break;
				case TYPE_CHAR:
					$$.value = ($1.value >= $3.value);
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			} else {
			/*Quadruples Code*/
			intercode_relop(&$$, &$1, &$3, OP_geq);
			/*End Quadruples Code*/
			}
		}
		/*Start and or operations*/
		|T_not expr
		{
			$$.calculated = 0;
			
			if(!$2.calculated && $2.type == typePointer(typeBoolean)) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $2.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$2.se = z->value.se;
				$2.type = typeBoolean;
				GENQUAD(OP_assign, x, y, z);
			}

			if ($2.se != NULL && !$2.calculated)
				conversion_from_expression_to_condition(&$2, &$2);
					
			if ($2.type == typeBoolean) {
				$$.type = typeBoolean;
				if ($2.calculated == 1 ) { //if const value
					$$.calculated = 1;
					$$.value = !($2.value);
				}
			}
			else yyerror("Expression must be of type Boolean\n");

			/*Quadruples Code*/
			$$.headTRUE = $2.headFALSE;
			$$.headFALSE = $2.headTRUE;
			/*End Quadruples Code*/

		}
		|T_unot expr
		{
			$$.calculated = 0;
			
			if (!$2.calculated && $2.type->kind == TYPE_POINTER) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $2.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$2.se = z->value.se;
				$2.type = $2.type->refType;
				GENQUAD(OP_assign, x, y, z);
			}

			if ($2.se != NULL && !$2.calculated)
				conversion_from_expression_to_condition(&$2, &$2);

			if ($2.type == typeBoolean) {
				$$.type = typeBoolean;
				if ($2.calculated == 1 ) {//if const value
					$$.calculated = 1;
					$$.value = !($2.value);
				}
			}
			else yyerror("Expression must be of type Boolean\n");

			/*Quadruples Code*/
			$$.headTRUE = $2.headFALSE;
			$$.headFALSE = $2.headTRUE;
			/*End Quadruples Code*/
		}

		|expr T_and
		{
			$<v>$.calculated = 0;
			
			if (!$1.calculated && $1.type->kind == TYPE_POINTER) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $1.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$1.se = z->value.se;
				$1.type = $1.type->refType;
				GENQUAD(OP_assign, x, y, z);
			}

			if ($1.se != NULL || $1.calculated)
				conversion_from_expression_to_condition(&$1, &$1);

			if ($1.type == typeBoolean) {
				$<v>$.type = typeBoolean;
				if ($1.calculated == 1 && $1.value == 0) {
					$<v>$.value = 0;
					$<v>$.calculated = 1;
				}
			}
			else yyerror("Type mismatch. ");

			/*Quadruples Code*/
			_BACKPATCH(&$1.headTRUE, quadNext);
			/*End Quadruples Code*/
		}
		expr
		{
			if (!$4.calculated && $4.type->kind == TYPE_POINTER) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $4.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$4.se = z->value.se;
				$4.type = $4.type->refType;
				GENQUAD(OP_assign, x, y, z);
			}

			if ($4.se != NULL || $1.calculated)
				conversion_from_expression_to_condition(&$4, &$4);

			if (!($<v>3.calculated == 1 && $4.value == 0)){
				if ($4.type == typeBoolean) {
					if ($4.calculated == 1 && $4.value != 1 ) {
						$$.value = 0;
						$$.calculated = 1;
					}
					else if ($4.calculated == 1 && $4.value == 1 &&
							$1.calculated == 1 && $1.value == 1 ){
						$$.value = 1;
						$$.calculated = 1;
					}
				}
				else yyerror("Type mismatch. ");
			}
			/*Quadruples Code*/
			$$.headTRUE = $4.headTRUE;
			$$.headFALSE = MERGE(&$1.headFALSE, &$4.headFALSE);
			printf("HEADTRUE\n");
			DisplayCList(&$$.headTRUE);
			printf("HEADFALSE\n");
			DisplayCList(&$$.headFALSE);
			/*End Quadruples Code*/

			$$.calculated = $1.calculated && $4.calculated;
		}
		|expr T_or
		{
			if (!$1.calculated && $1.type->kind == TYPE_POINTER) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $1.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$1.se = z->value.se;
				$1.type = $1.type->refType;
				GENQUAD(OP_assign, x, y, z);
			}

			if ($1.se != NULL)
				conversion_from_expression_to_condition(&$1, &$1);

			$<v>$.calculated = 0;
			if ($1.type == typeBoolean) {
				if ($1.calculated == 1 && $1.value == 1) {
					$<v>$.value = 1;
					$<v>$.calculated = 1;
					$<v>$.type = typeBoolean;
				}
			}
			else yyerror("Type mismatch. ");

			/*Quadruples Code*/
			_BACKPATCH(&$1.headFALSE, quadNext);
			/*End Quadruples Code*/
		}
		expr
		{
			if (!$4.calculated && $4.type->kind == TYPE_POINTER) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $4.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$4.se = z->value.se;
				$4.type = $4.type->refType;
				GENQUAD(OP_assign, x, y, z);
			}

			if ($4.se != NULL)
				conversion_from_expression_to_condition(&$4, &$4);

			if ($4.type == typeBoolean) {
				$$.type = typeBoolean;
				if ($4.calculated == 1 && $4.value == 1) {
					$<v>3.value = 1;
					$<v>3.calculated = 1;
					break;
				}
				else if ($4.calculated == 1 && $4.value == 1 &&
						 $1.calculated == 1 && $1.value == 1) {
					$<v>3.value = 0;
					$<v>3.calculated = 1;
				}
			}
			else yyerror("Type mismatch. ");

			/*Quadruples Code*/
			$$.headFALSE = $4.headFALSE;
			$$.headTRUE = MERGE(&$1.headTRUE, &$4.headTRUE);
			/*End Quadruples Code*/

			$$.calculated = $1.calculated && $4.calculated;
		}
		|expr  T_band
		{
			/*TODO*/
		}
		expr
		{
			/*TODO*/
		}

		|expr  T_bor
		{
			/*TODO*/
		}
		expr
		{
			/*TODO*/
		}
		;
l_value		:"id"
		{
			PushL(var_type, &l_value_stack);
			PushSE(se, &se_stack);
			PushArray(array, &array_stack);

			se = lookupEntry($1, LOOKUP_ALL_SCOPES, false);
			$<v>$.se = se;
			if (se == NULL)
				yyerror("NameError. Name is not defined");
			else if (se->entryType == ENTRY_VARIABLE) {
				var_type = se->u.eVariable.type;
				$<v>$.calculated = 0;
			}
			else if (se->entryType == ENTRY_PARAMETER) {
				var_type = se->u.eParameter.type;
				$<v>$.calculated = 0;
			}
			else if (se->entryType == ENTRY_CONSTANT) {
				var_type = se->u.eConstant.type;
				$<v>$.calculated = 1;
			}
			else yyerror("Strange error. Lvalue not variable or parameter or costant variable");
			
			/*We assume l_value is not array, if not we change that later*/
			array = 0 ;
		}
		more_expr_br
		{
			$$.se = se;
			$$.calculated = $<v>2.calculated;
			/*array,a,b,c return a pointer to a[i] in c, 
			 *otherwise we have no typePointer
			 */
			if (array) 
				var_type = typePointer(var_type);
			$$.type = var_type;
			se = PopSE(&se_stack);
			var_type = PopL(&l_value_stack);
			array = PopArray(&array_stack);
		}
		;
more_expr_br	:/*empty*/
		{
		}
		|'['
		{
			array = 1;
			if (var_type->kind == TYPE_ARRAY || var_type->kind == TYPE_IARRAY)
				var_type = var_type->refType;
			else yyerror("IndexError: No such element of array");
		}
		expr ']'
		{
			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));

			if ($<v>3.calculated == 1) {
				y->type = QUAD_INTEGER;
				y->value.intval = $<v>3.value;
			}
			else {
				if ($<v>3.type->kind == TYPE_POINTER)
					y->type = QUAD_POINTER;
				else 
					y->type = QUAD_SE;
				y->value.se = $<v>3.se;
			}
			x->type = QUAD_SE;
			x->value.se = se;
			z->type = QUAD_SE;
			z->value.se = newTemporary(typePointer(var_type));
			se = z->value.se;
			GENQUAD(OP_ARRAY, x, y, z);
			/*End Quadruples code*/
		}
		more_expr_br
		;
call 	:"id" '('
		{
			Push(f_se, &top);
			Push(current, &top);
			f_se = lookupEntry($1, LOOKUP_ALL_SCOPES, false);
			if (f_se == NULL) {
				yyerror("NameError. function/procedure is not defined ");
			} else if (f_se->entryType != ENTRY_FUNCTION )
				yyerror("Element that is called in not a function/procedure");
			current = f_se->u.eFunction.firstArgument;
		}
		args ')'
		{
			$$ = f_se->u.eFunction.resultType;

			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));

			printf("END OF CALL \n");

			if (!equalType($$, typeVoid)) {
				printf("END OF CALL 1 \n");
				PushSE(call_res, &call_result_stack);
				call_res = newTemporary($$);//CALL PLACE
				x->type = QUAD_SE;
				x->value.se = call_res;
				x->type =QUAD_MODE;
				x->mode = RET;
				y->type = QUAD_SE;
				y->value.se = call_res;
				z->type = QUAD_EMPTY;
				GENQUAD(OP_PAR, x, y, z);
			}
			printf("END OF CALL 2 \n");

			x = (quad *) new(sizeof(quad));
			y = (quad *) new(sizeof(quad));
			z = (quad *) new(sizeof(quad));
			x->type = QUAD_EMPTY;
			y->type = QUAD_EMPTY;
			z->type = QUAD_SE;
			z->value.se = f_se;
			GENQUAD(OP_CALL, x, y, z);
			/*End Quadruples code*/

			current = Pop(&top);
			f_se = Pop(&top);
		}
		;
more_expr: /*empty*/
		{
			current = current->u.eParameter.next;
			if (current != NULL)
				yyerror("Fewer arguments than expected in redeclaration of the function");
		}
		| ','  expr
		{
			current = current->u.eParameter.next;
			if (current == NULL )
				yyerror("More arguments than expected in redeclaration of function ");
			if (current->u.eParameter.mode == PASS_BY_VALUE) {
				compatible_assignment(current->u.eParameter.type, $2.type);
			}
			else if (current->u.eParameter.mode == PASS_BY_REFERENCE)
				compatible_PASS_BY_REFERENCE(current->u.eParameter.type, $2.type);
			else 
				yyerror("Error here\n");

			/*Quadruples code*/
			/*CHECK THIS LATER*/
			/*if ($2.type == typeBoolean) {
				$2.se = conversion_from_condition_to_expression(&$2);
			}
			*/
			intercode_PAR_op(&current, &$2);
			/*End Quadruples code*/

		}
		more_expr
		;
args	:/*empty*/
		{
			if (current != NULL)
				yyerror("Fewer arguments than expected in redeclaration of the function ");
		}
		|expr
		{
			if (current == NULL )
				yyerror("More arguments than expected in redeclaration of function ");
			if (current->u.eParameter.mode == PASS_BY_VALUE) {
				compatible_assignment(current->u.eParameter.type, $1.type);
			}
			else if (current->u.eParameter.mode == PASS_BY_REFERENCE) {
				compatible_PASS_BY_REFERENCE(current->u.eParameter.type, $1.type);
			} else {
				yyerror("Something went wrong\n");
			}
			/*Quadruples code*/
			/*CHECK THIS LATER*/
			/*if ($1.type == typeBoolean) {
				if (!$1.calculated) 
					$1.se = conversion_from_condition_to_expression(&$1);
				else {
            		_BACKPATCH(&$1.headTRUE, quadNext);
            		_BACKPATCH(&$1.headFALSE, quadNext);
				}
			}
			*/
			intercode_PAR_op(&current, &$1);
			/*End Quadruples code*/
		}
		more_expr
		;
block 		:'{'
		{
			openScope();
		}
		inner_block '}'
		{
			closeScope();
			$$ = $3;
		}
		;
inner_block	:/*empty*/
		|local_def
		{
			$<headNEXT>$ = $1;
			_BACKPATCH(&$<headNEXT>$, quadNext);
		}
		inner_block
		|stmt
		{
			$<headNEXT>$ = $1;
			_BACKPATCH(&$<headNEXT>$, quadNext);
			printf("_BACKPATCH %d\n", quadNext);
		}
		inner_block
		{
			$$ = $1;

		}
		;
local_def	:const_def
		{
		$$ = EMPTYLIST();
		}
		|var_def
		{
		$$ = EMPTYLIST();
		}
		;
stmt		:';'
		{
		$$ = EMPTYLIST();
		}
		|l_value '=' expr ';'
		{
			compatible_assignment($1.type, $3.type);
			/*Quadruples code*/
			if ($3.type == TYPE_VOID)
				yyerror("Cannot get result of a void function");
			intercode_assign_op(&$1, &$3);
			$$ = EMPTYLIST();
			/*End Quadruples code*/

		}
		|l_value "+=" expr ';'
		{
			compatible_assignment($1.type, compatible_arithmetic_OP($1.type, $3.type));
			/*Quadruples code*/
			Vinfo v ;
			intercode_arithmetic_op(&v, &$1, &$3, OP_PLUS);
			intercode_assign_op(&$1, &v);
			$$= EMPTYLIST();
			/*End Quadruples code*/
		}

		|l_value "-=" expr ';'
		{
			compatible_assignment($1.type, compatible_arithmetic_OP($1.type, $3.type));

			/*Quadruples code*/
			Vinfo v ;
			intercode_arithmetic_op(&v, &$1, &$3, OP_MINUS);
			intercode_assign_op(&$1, &v);
			$$= EMPTYLIST();
			/*End Quadruples code*/
		}
		|l_value "*=" expr ';'
		{
			compatible_assignment($1.type, compatible_arithmetic_OP($1.type, $3.type));

			/*Quadruples code*/
			Vinfo v ;
			intercode_arithmetic_op(&v, &$1, &$3, OP_bmul);
			intercode_assign_op(&$1, &v);
			$$= EMPTYLIST();
			/*End Quadruples code*/
		}
		|l_value "/=" expr ';'
		{
			if ($3.calculated == 1 && (($3.value == 0 && $3.type == typeInteger) || ($3.floatval == 0 && $3.type == typeReal)))
				yyerror("ZeroDivisionError: integer division or modulo by zero.");
			compatible_assignment($1.type, compatible_arithmetic_OP($1.type, $3.type));

			/*Quadruples code*/
			Vinfo v ;
			intercode_arithmetic_op(&v, &$1, &$3, OP_bdiv);
			intercode_assign_op(&$1, &v);
			$$= EMPTYLIST();
			/*End Quadruples code*/
		}
		|l_value "%=" expr
		{
			if ($3.calculated == 1 && $3.value == 0 && $3.type == typeInteger)
				yyerror("ZeroDivisionError: integer division or modulo by zero.");
			compatible_assignment($1.type, compatible_arithmetic_OP($1.type, $3.type));

			/*Quadruples code*/
			Vinfo v ;
			intercode_arithmetic_op(&v, &$1, &$3, OP_bmod);
			intercode_assign_op(&$1, &v);
			$$= EMPTYLIST();
			/*End Quadruples code*/
		}
		|l_value "++" ';'
		{
			compatible_arithmetic_OP($1.type, typeInteger);
			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));
			x->type = QUAD_SE;
			x->value.se = $1.se;
			y->type = QUAD_INTEGER;
			y->value.intval = 1;
			z->type = QUAD_SE;
			z->value.se = $1.se;
			GENQUAD(OP_PLUS, x, y, z);
			$$= EMPTYLIST();
			/*End Quadruples code*/
		}
		|l_value "--" ';'
		{
			compatible_arithmetic_OP($1.type, typeInteger);
			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));
			x->type = QUAD_SE;
			x->value.se = $1.se;
			y->type = QUAD_INTEGER;
			y->value.intval = 1;
			z->type = QUAD_SE;
			z->value.se = $1.se;
			GENQUAD(OP_MINUS, x, y, z);
			$$= EMPTYLIST();
			/*End Quadruples code*/
		}
		|l_value
		{
			$$ = EMPTYLIST();
		}
		|call ';'
		{
			/*Quadruples code*/
			$$ = EMPTYLIST();
			/*End Quadruples code*/
		}
		|"if" '(' expr ')'
		{
			if (!$3.calculated && $3.type->kind == TYPE_POINTER) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $3.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$3.se = z->value.se;
				$3.type = $3.type->refType;
				GENQUAD(OP_assign, x, y, z);
			}

			if ($3.se != NULL || $3.calculated)
				conversion_from_expression_to_condition(&$3, &$3);


			if (!equalType($3.type, typeBoolean))
				yyerror("Type mismatch. Condition must be of type bool");

			/*Quadruples code*/
			Push_N(condFALSE, &head_condFALSE);
			Push_N(L1, &head_L1);
			Push_N(L2, &head_L2);
			condFALSE = $3.headFALSE;
			_BACKPATCH(&$3.headTRUE, quadNext);
			L1 = $3.headFALSE;
			L2 = EMPTYLIST();
			/*End Quadruples code*/
		}
		stmt thenclause
		{
			/*Quadruples code*/
			condition *aux = MERGE(&L1, &L2);
			$$ = MERGE(&aux, &$6);
			printf("Will display then clause next list :");
			DisplayCList(&$$);
			L2 = Pop_N(&head_L2);
			L1 = Pop_N(&head_L1);
			condFALSE = Pop_N(&head_condFALSE);
			/*End Quadruples code*/
		}
		|"while"
		{
			/*Quadruples code*/
			Push_W(while_backQUAD, &while_stack);
			while_backQUAD = quadNext;
			/*End Quadruples code*/
		}
		'(' expr ')'
		{
				
			if(!$4.calculated && $4.type == typePointer(typeBoolean)) { 	
				quad *x = (quad *) new(sizeof(quad));
				quad *y = (quad *) new(sizeof(quad));
				quad *z = (quad *) new(sizeof(quad));
				x->value.se = $4.se;
				x->type = QUAD_POINTER;
				y->type = QUAD_EMPTY;
				z->type = QUAD_SE;
				z->value.se = newTemporary(typeBoolean);
				$4.se = z->value.se;
				$4.type = typeBoolean;
				GENQUAD(OP_assign, x, y, z);
			}

			/*NOT SURE IF THATS NEEDED HERE*/
			if ($4.se != NULL)
				printf("NAME is : %s\n", $4.se->id),conversion_from_expression_to_condition(&$4, &$4);
			
			if ($4.type != typeBoolean)
				yyerror("Expression must be of type Boolean\n");

			/*Quadruples code*/
			_BACKPATCH(&$4.headTRUE, quadNext);
			/*End Quadruples code*/
		}
		stmt
		{
			/*Quadruples code*/
			_BACKPATCH(&$7, while_backQUAD);

			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));
			x->type = QUAD_EMPTY;
			y->type = QUAD_EMPTY;
			z->type = QUAD_TAG;
			z->value.intval = while_backQUAD;
			GENQUAD(OP_JUMP, x, y, z);
			$$ = $4.headFALSE;
			while_backQUAD = Pop_W(&while_stack);
			/*End Quadruples code*/
		}
		|"FOR" '(' "id"
		{
			se = lookupEntry($3, LOOKUP_ALL_SCOPES, false);
			if (se == NULL)
				yyerror("NameError. Name is not defined");
			else if (se->entryType == ENTRY_VARIABLE) {
				var_type = se->u.eVariable.type;
			}
			else if (se->entryType == ENTRY_PARAMETER)
				var_type = se->u.eParameter.type;
			if (!equalType(var_type,typeInteger)) {
				yyerror("Type mismatch. FOR (i, r ) s : i must be of type integer");
			}

			/*Quadruples code*/
			Push_F(for_backQUAD, for_counter, se, &for_stack);
			for_counter = newTemporary(typeInteger);
			/*End Quadruples code*/
		}
		',' expr
		{

		/*Quadruples code*/
		/*Assign initial value to the for_counter and iterator*/
		/*Starting point is not always a constant, FIX THIS*/
		quad *x = (quad *) new(sizeof(quad));
		quad *y = (quad *) new(sizeof(quad));
		quad *z = (quad *) new(sizeof(quad));
		z->type = QUAD_SE;
		z->value.se = for_counter;
		x->type = QUAD_INTEGER;
		x->value.intval = $6.value;
		y->type = QUAD_EMPTY;
		GENQUAD(OP_assign, x, y, z);
			
		x = (quad *) new(sizeof(quad));
		y = (quad *) new(sizeof(quad));
		z = (quad *) new(sizeof(quad));

		x->type = QUAD_SE;
		x->value.se = for_counter;
		y->type = QUAD_EMPTY;
		z->type = QUAD_SE;
		z->value.se = se;
		GENQUAD(OP_assign, x, y, z);
		/*End Quadruples code*/
		}
		to expr step
		{
			if (!equalType($9.type, typeInteger) || !equalType($6.type, typeInteger))
				yyerror("Type mismatch. Range must be of type integer");
			/*Quadruples code*/
			for_backQUAD = quadNext;

			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));

			Vinfo v1;
			Vinfo v2;
			Vinfo vv;
			v1.type = typeInteger;
			v1.se = for_counter;
			v2 = $9;
			if ($8 == _TO)
				intercode_relop(&vv, &v1, &v2, OP_leq);
			else if ($8 == _DOWN_TO)
				intercode_relop(&vv, &v1, &v2, OP_geq);
			_BACKPATCH(&vv.headTRUE, quadNext);
			$<headNEXT>$= vv.headFALSE;

			/*i = $1*/
			x = (quad *) new(sizeof(quad));
			y = (quad *) new(sizeof(quad));
			z = (quad *) new(sizeof(quad));

			x->type = QUAD_SE;
			x->value.se = for_counter;
			y->type = QUAD_EMPTY;
			z->type = QUAD_SE;
			z->value.se = se;
			GENQUAD(OP_assign, x, y, z);
			
			/*add step*/
			if ($8 == _TO) {
				Vinfo x;
				x.type = typeInteger;
				x.se = for_counter;
				x.calculated = 0;
				
				Vinfo z;
				z.type = typeInteger;
				z.se = for_counter;
				z.calculated = 0;

				intercode_arithmetic_op_givenRET(&x, &$10, &z, OP_PLUS);
			}
			else if ($8 == _DOWN_TO) {
				Vinfo x;
				x.type = typeInteger;
				x.se = for_counter;
				x.calculated = 0;
				
				intercode_arithmetic_op_givenRET(&x, &$10, &x, OP_MINUS);
			}

			/*End Quadruples code*/
		}
		')' stmt
		{
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));

			/*Quadruples code*/
			_BACKPATCH(&$13, for_backQUAD);
			x = (quad *) new(sizeof(quad));
			y = (quad *) new(sizeof(quad));
			z = (quad *) new(sizeof(quad));
			x->type = QUAD_EMPTY;
			y->type = QUAD_EMPTY;
			z->type = QUAD_TAG;
			z->value.intval = for_backQUAD;
			GENQUAD(OP_JUMP, x, y, z);
			$$ = $<headNEXT>11;
			for_list = $$;
			node_F f = Pop_F(&for_stack);
			for_counter = f.for_counter;
			for_backQUAD = f.for_backQUAD;
			se = f.for_se;
			/*End Quadruples code*/
			
			
			/*TODO : Store back the for_counter to the iterator*/
		}
		|"do"
		{
			/*Quadruples code*/
			Push_W(while_backQUAD, &while_stack);
			while_backQUAD = quadNext;
			/*End Quadruples code*/
		}
		stmt "while" '(' expr ')' ';'
		{
			if (!equalType($6.type, typeBoolean))
				yyerror("Type mismatch. Condition must be of type bool");
			/*Quadruples code*/
			_BACKPATCH(&$3, while_backQUAD);
			_BACKPATCH(&$6.headFALSE, while_backQUAD);
			$$ = $6.headTRUE;
				while_backQUAD = Pop_W(&while_stack);
			/*End Quadruples code*/
		}
		|"break" ';'
		{
			/*Quadruples code*/
			$$ = EMPTYLIST();
			/*End Quadruples code*/
		}
		|"continue" ';'
		{
			/*Quadruples code*/
			$$ = EMPTYLIST();
			/*End Quadruples code*/
		}
		|"return" ';'
		{
			/*Quadruples code*/
			$$= EMPTYLIST();
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));
			x->type = QUAD_EMPTY;
			y->type = QUAD_EMPTY;
			z->type = QUAD_EMPTY;
			GENQUAD(OP_RET,x,y,z);
			/*End Quadruples code*/
		}
		|"return" expr ';'
		{
			/*Quadruples code*/
			$$= EMPTYLIST();
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));

			if ($2.calculated) {
				if ($2.type == typeChar) {
					x->type = QUAD_CHAR;
					x->value.intval = $2.value;
				}
				else if ($2.type == typeInteger) {
					x->type = QUAD_INTEGER;
					x->value.intval = $2.value;
				}
				else if ($2.type == typeBoolean) {
					//se = conversion_from_condition_to_expression(&$2);
					x->type = QUAD_BOOL;
					x->value.se = $2.se;
				}
				else if ($2.type == typeReal) {
					x->type = QUAD_REAL;
					x->value.floatval = $2.floatval;
				}
			} else { 
				if ($2.type->kind == TYPE_POINTER)
					x->type = QUAD_POINTER;
				else
					x->type = QUAD_SE;
				x->value.se = $2.se;
			
				if ($2.type == typeBoolean) {
					se = conversion_from_condition_to_expression(&$2);
					x->value.se = se;
				}
			}
			y->type = QUAD_EMPTY;
			z->type = QUAD_EMPTY;
			GENQUAD(OP_RETV, x, y, z);
			/*End Quadruples code*/
		}
		|block
		{
			$$ = $1;	/*stmt.NEXT = block.NEXT*/
		}
		|write '('format  more_format ')' ';'
		{
			/*Quadruples code*/
			$$ = EMPTYLIST();;
			/*End Quadruples code*/
		}
		|write '(' ')' ';'
		{
			$$ = EMPTYLIST();
		}
		;
thenclause:	/*empty*/
		{
		}	%prec "then"
		|"else"
		{
			/*Quadruples code*/
			quad *x = (quad *) new(sizeof(quad));
			quad *y = (quad *) new(sizeof(quad));
			quad *z = (quad *) new(sizeof(quad));

			x->type = QUAD_EMPTY;
			y->type = QUAD_EMPTY;
			z->type = QUAD_TOFILL;
			GENQUAD(OP_JUMP, x, y, z);

			L1 = MAKELIST(z, quadNext);
			_BACKPATCH(&condFALSE, quadNext);
			/*End Quadruples code*/
		}
		stmt
		{
			/*Quadruples code*/
			L2 = $3;
			/*End Quadruples code*/
		}
		;
more_format: /*empty*/
		{
		}
		|',' format more_format /*Check this right recursion*/
		;
write	:"WRITE"
		|"WRITELN"
		|"WRITESP"
		|"WRITESPLN"
		;
format	:expr 
		{	
			printf("DEBUG HERE\n");
			SymbolEntry *fe;
			SymbolEntry *currentA;
			
			if (equalType($1.type, typeInteger) ||
				equalType($1.type, typePointer(typeInteger)))
				fe = lookupEntry("WRITE_INT", LOOKUP_ALL_SCOPES, false);
			else if (equalType($1.type, typeBoolean) ||
				equalType($1.type, typePointer(typeBoolean)))
				fe = lookupEntry("WRITE_BOOL", LOOKUP_ALL_SCOPES, false);
			else if (equalType($1.type, typeChar) || 
				equalType($1.type, typePointer(typeChar)))
				fe = lookupEntry("WRITE_CHAR", LOOKUP_ALL_SCOPES, false);
			else if (equalType($1.type, typeReal) || 
				equalType($1.type, typePointer(typeReal)))
				fe = lookupEntry("WRITE_REAL", LOOKUP_ALL_SCOPES, false);
			else if (($1.type->kind == TYPE_ARRAY || $1.type->kind ==
				TYPE_IARRAY) && equalType($1.type->refType,	typeChar))
				fe = lookupEntry("WRITE_STRING", LOOKUP_ALL_SCOPES, false);
			else 
				yyerror("Unknown type");
			/*FIX THIS , what we have a string from an array as parameter*/
            currentA = fe->u.eFunction.firstArgument;
			
			intercode_PAR_op(&currentA, &$1);

            currentA = currentA->u.eParameter.next;

			Vinfo a;
			a.type = typeInteger;
			a.calculated = 1;
			a.value = 0;
			intercode_PAR_op(&currentA, &a);
			
			/*Quadruples code*/
            quad *x = (quad *) new(sizeof(quad));
            quad *y = (quad *) new(sizeof(quad));
            quad *z = (quad *) new(sizeof(quad));

            x->type = QUAD_EMPTY;
            y->type = QUAD_EMPTY;
            z->type = QUAD_SE;
            z->value.se = fe;
            GENQUAD(OP_CALL, x, y, z);
            /*End Quadruples code*/
		}
		| "FORM" '(' expr ',' expr ')'|  "FORM" '(' expr ',' expr ',' expr ')';
to		:"TO"
		{
			$$ =_TO;
		}
		| "DOWNTO"
		{
			$$= _DOWN_TO;
		}
		;
step 	:"STEP" expr
		{
			if (!equalType($2.type, typeInteger))
				yyerror("Type mismatch. Step must be of type integer");
			if ($2.calculated && $2.value <= 0)
				yyerror("Step mst be a positive interger");
			$$ = $2;
		}
		|/*empty step*/
		{
			$$.calculated = 1;
			$$.value = 1;
			$$.type = typeInteger;
		}
		;

%%


void ERROR (const char *fmt, ...);

void yyerror (const char *msg)
{
	ERROR("parser said, %s", msg);
}

int main (void)
{
	/*Define all routines that exist in the pazcal library*/
	se_stack = NULL;
	l_value_stack = NULL;
	top = NULL;
	head_quad = NULL;
	current_quad = head_quad;

	initSymbolTable(256);
	openScope();
	define_routines();
	yyparse();
	closeScope();
	destroySymbolTable();
	printf("\n\n");
	DisplayQuads(&head_quad);
	return 0;
}


