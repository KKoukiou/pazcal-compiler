/***	DEFINITION SECTION 	***/
%{
#include <string.h>
#include <stdarg.h>
#include <stdlib.h> /*strtold*/
#include "symbol.h"
#include "inter.h"
#include "parser.h"

#define T_eof 		0

int lineno = 1;

void ERROR (const char * fmt, ...);

%}		

%option noyywrap

L [A-Za-z]
D [0-9]
W [\t\n]
E [\n\t\r\0\\\'\"]
DEL [\&\;\.\(\)\:\,\[\]\{\}]
C ([^'"\\\n]|\\.)

%x COMMENT

%%


"and"		{return T_and;		}
"continue"	{return T_continue; }
"FOR"		{return T_for; 		}
"NEXT"		{return T_next;		}	
"return"	{return T_return;	}
"WRITE" 	{return T_write;	}
"bool"		{return T_bool;		}
"default"	{return T_default;	}
"FORM"		{return T_form;		}
"not"		{return T_not;		}
"STEP"		{return T_step;		}
"WRITELN"	{return T_writeln;	}
"break" 	{return T_break; 	}
"do"		{return T_do;		}
"FUNC"		{return T_func;		}
"or"		{return T_or;		}
"switch"	{return T_switch;	}
"WRITESP"	{return T_writesp;	}
"case"		{return T_case; 	}
"DOWNTO"	{return T_downto;	}
"if"		{return T_if;		}
"PROC"		{return T_proc;		}
"TO"		{return T_to;		}
"WRITESPLN"	{return T_writespln;}
"char"		{return T_char;		}
"else"		{return T_else;		}
"int"		{return T_int;		}
"PROGRAM"	{return T_program;	}
"true"		{return T_true;		}
"const"		{return T_const;	}
"false"		{return T_false;	}
"MOD"		{return T_mod;		}
"REAL"		{return T_real;		}
"while"		{return T_while;	}

"!"		{return T_unot;		}
"+"		{return '+';		}
"-"		{return '-';	}
"*"		{return T_bmul;		}
"/"		{return T_bdiv;		}
"%"		{return T_bmod;		}
"=="		{return T_eq;		}
"!="		{return T_neq;		}
"<"		{return T_less;		}
">"		{return T_greater;	}
"<="		{return T_leq;		}
">="		{return T_geq;		}
"&&"		{return T_band;		}
"||"		{return T_bor;		}
"="		{return '=';		}
"+="		{return T_pe;		}
"-="		{return T_me;		}
"*="  		{return T_mule;		}
"/="		{return	T_dive;		}
"%=" 		{return T_mode;		} 
"++"		{return T_plusplus;	}
"--"		{return T_minusminus;	}



{DEL} 				{return yytext[0]; }
(\')({L}|{D}|{DEL}|{C})(\') 			{ 	
											printf("%c%c\n", yytext[1],
											yytext[2]);
											if (yytext[1] == '\\') {
												if (yytext[2] == 'n')	
													yylval.num = 10;
												else if (yytext[2] == 't')
													yylval.num = 9;
											} else { 
												yylval.num = yytext[1];
												/*More work needed here*/
											}
											return T_char_const;	
										}
(\")({DEL}*{L}*{D}*[\+\-\*\/\=\!\<\>\?\@\#\$\%\^\&\\ ]*)*(\") {yylval.n = yytext; return T_string_const;	}
([1-9]([0-9])*)|[0]		{yylval.num = atoi(yytext);return T_int_const;	}
{D}+(\.{D}+(e\-?{D}+)?) 	{yylval.real = strtold(yytext, NULL);return T_float_const;	}
{L}({L}|{D}|_)* 		{yylval.n = strdup(yytext); return T_id;		}
{D}+(\.{D}*(e\-?{D}+)?)? 	{return T_const;	}
[ \t]+				{ /*nothing*/		}
[\n]				{ lineno++;		}
<<EOF>> 			{ return T_eof; 	}
(\/\/[^\n]*)		{ /*nothing*/}
"/*"		{BEGIN(COMMENT);	}
<COMMENT>"*/"	{BEGIN(INITIAL);	}
<COMMENT>\n	{lineno++;		}	
<COMMENT>"*"	{			}
<COMMENT>[^*\n]+ {			}	

.       { ERROR("illegal character: '%c' (ASCII: %d)",
		                yytext[0], yytext[0]); }


%%


void ERROR (const char * fmt, ...)
{
   va_list ap;

   va_start(ap, fmt);
   fprintf(stderr, "Line %d, error: ", lineno);
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   va_end(ap);
   exit(1);
}


