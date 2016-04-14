/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_id = 258,
    T_for = 259,
    T_next = 260,
    T_return = 261,
    T_write = 262,
    T_bool = 263,
    T_default = 264,
    T_form = 265,
    T_step = 266,
    T_writeln = 267,
    T_break = 268,
    T_do = 269,
    T_func = 270,
    T_switch = 271,
    T_writesp = 272,
    T_case = 273,
    T_downto = 274,
    T_if = 275,
    T_proc = 276,
    T_to = 277,
    T_writespln = 278,
    T_char = 279,
    T_else = 280,
    T_int = 281,
    T_program = 282,
    T_true = 283,
    T_const = 284,
    T_false = 285,
    T_real = 286,
    T_while = 287,
    T_continue = 288,
    T_mod = 289,
    T_int_const = 290,
    T_float_const = 291,
    T_char_const = 292,
    T_string_const = 293,
    T_pe = 294,
    T_me = 295,
    T_mule = 296,
    T_dive = 297,
    T_mode = 298,
    T_plusplus = 299,
    T_minusminus = 300,
    T_or = 301,
    T_bor = 302,
    T_band = 303,
    T_and = 304,
    T_eq = 305,
    T_neq = 306,
    T_less = 307,
    T_greater = 308,
    T_leq = 309,
    T_geq = 310,
    T_bmul = 311,
    T_bdiv = 312,
    T_bmod = 313,
    T_uminus = 314,
    T_uplus = 315,
    T_unot = 316,
    T_not = 317
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 53 "parser.y" /* yacc.c:1909  */

	char *n;
	int num;
	long double real;
	Type t;
	Vinfo v;
	condition *headNEXT;
	TO_TYPE to;

#line 127 "parser.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
