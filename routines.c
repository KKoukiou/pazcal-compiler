#include "symbol.h"

void define_routines(void)
{	
	SymbolEntry *se;
	const char *name;

	name = "READ_INT";
	se = newFunction(name);
	openScope();
    endFunctionHeader(se, typeInteger);
	closeScope();

	name = "READ_BOOL";
	se = newFunction(name);
	openScope();
    endFunctionHeader(se, typeBoolean);
	closeScope();
	
	name = "getchar";
	se = newFunction(name);
	openScope();
    endFunctionHeader(se, typeInteger);
	closeScope();

	name = "READ_REAL";
	se = newFunction(name);
	openScope();
    endFunctionHeader(se, typeReal);
	closeScope();

	name = "READ_STRING";
	se = newFunction(name);
	openScope();
	newParameter("par1", typeInteger, PASS_BY_VALUE, se);
	newParameter("par2", typeIArray(typeChar), PASS_BY_REFERENCE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

	name = "putchar";
	se = newFunction(name);
	openScope();
	newParameter("par", typeChar, PASS_BY_VALUE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

	name = "puts";
	se = newFunction(name);
	openScope();
	newParameter("par", typeIArray(typeChar), PASS_BY_REFERENCE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

	name = "WRITE_INT";
	se = newFunction(name);
	openScope();
	newParameter("par1", typeInteger, PASS_BY_VALUE, se);
	newParameter("par2", typeInteger, PASS_BY_VALUE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

	name = "WRITE_BOOL";
	se = newFunction(name);
	openScope();
	newParameter("par1", typeBoolean, PASS_BY_VALUE, se);
	newParameter("par2", typeInteger, PASS_BY_VALUE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

	name = "WRITE_CHAR";
	se = newFunction(name);
	openScope();
	newParameter("par1", typeChar, PASS_BY_VALUE, se);
	newParameter("par2", typeInteger, PASS_BY_VALUE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

	name = "WRITE_REAL";
	se = newFunction(name);
	openScope();
	newParameter("par1", typeReal, PASS_BY_VALUE, se);
	newParameter("par2", typeInteger, PASS_BY_VALUE, se);
	newParameter("par3", typeInteger, PASS_BY_VALUE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

	name = "WRITE_STRING";
	se = newFunction(name);
	openScope();
	newParameter("par1", typeIArray(typeChar), PASS_BY_REFERENCE, se);
	newParameter("par2", typeInteger, PASS_BY_VALUE, se);
    endFunctionHeader(se, typeVoid);
	closeScope();

}
