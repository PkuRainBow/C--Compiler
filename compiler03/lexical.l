
digit			[0-9]
letter		[a-zA-Z_]
exp	[Ee][+-]?{digit}+

%option yylineno
%{
#include "syntax.tab.h"
#include "./include/compiler.h"
extern int is_error;
int yycolumn=1;
void alloc_node(int lineno, char *text);

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;\
yylloc.first_column = yycolumn;yylloc.last_column = yycolumn + yyleng - 1;\
yycolumn += yyleng;

%}

%%

"int"			{ alloc_node(yylineno, "TYPE"); strncpy(yylval.node->ID, "int",32);     return(TYPE); }
"float"			{ alloc_node(yylineno, "TYPE"); strncpy(yylval.node->ID, "float",32);     return(TYPE); }
"return"		{ alloc_node(yylineno, "RETURN"); return(RETURN); }
"struct"		{ alloc_node(yylineno, "STRUCT"); return(STRUCT); }
"if"			{ alloc_node(yylineno, "IF"); return(IF); }
"else"			{ alloc_node(yylineno, "ELSE"); return(ELSE); }
"while"			{ alloc_node(yylineno, "WHILE"); return(WHILE); }

{letter}({letter}|{digit})*		{ alloc_node(yylineno, "ID"); strncpy(yylval.node->ID, yytext,32); return(ID); }


{digit}+{exp}		{ alloc_node(yylineno, "FLOAT"); yylval.node->float_value = atof(yytext);  return(FLOAT); }
{digit}*"."{digit}+({exp})?	{ alloc_node(yylineno, "FLOAT"); yylval.node->float_value = atof(yytext); return(FLOAT); }
{digit}+"."{digit}*({exp})?	{ alloc_node(yylineno, "FLOAT"); yylval.node->float_value = atof(yytext); return(FLOAT); }
{digit}+		{ alloc_node(yylineno, "INT"); yylval.node->int_value = atoi(yytext); return(INT); }

"&&"			{ alloc_node(yylineno, "AND"); return(AND); }
"||"			{ alloc_node(yylineno, "OR"); return(OR); }
"<"			{ alloc_node(yylineno, "RELOP"); strncpy(yylval.node->ID, yytext, 32);return(RELOP); }
">"			{ alloc_node(yylineno, "RELOP"); strncpy(yylval.node->ID, yytext, 32);return(RELOP); }
"<="			{ alloc_node(yylineno, "RELOP"); strncpy(yylval.node->ID, yytext, 32);return(RELOP); }
">="			{ alloc_node(yylineno, "RELOP"); strncpy(yylval.node->ID, yytext, 32);return(RELOP); }
"=="			{ alloc_node(yylineno, "RELOP"); strncpy(yylval.node->ID, yytext, 32);return(RELOP); }
"!="			{ alloc_node(yylineno, "RELOP"); strncpy(yylval.node->ID, yytext, 32);return(RELOP); }
";"			{ alloc_node(yylineno, "SEMI"); return(SEMI); }
"{"			{ alloc_node(yylineno, "LC"); return(LC); }
"}"			{ alloc_node(yylineno, "RC");  return(RC); }
","			{ alloc_node(yylineno, "COMMA");  return(COMMA); }
"="			{ alloc_node(yylineno, "ASSIGNOP");  return(ASSIGNOP); }
"("			{ alloc_node(yylineno, "LP");  return(LP); }
")"			{ alloc_node(yylineno, "RP");  return(RP); }
"["			{ alloc_node(yylineno, "LB");  return(LB); }
"]"			{ alloc_node(yylineno, "RB");  return(RB); }
"."			{ alloc_node(yylineno, "DOT");  return(DOT); }
"!"			{ alloc_node(yylineno, "NOT"); return(NOT); }
"-"			{ alloc_node(yylineno, "MINUS");  return(MINUS); }
"+"			{ alloc_node(yylineno, "PLUS");  return(PLUS); }
"*"			{ alloc_node(yylineno, "STAR"); return(STAR); }
"/"			{ alloc_node(yylineno, "DIV");  return(DIV); }

\n			{yycolumn = 1;}
[ \t\v\f]		{ }
.			{ is_error = 1; printf("Error type A at line %d: Mysterious character '%s' \n", yylineno, yytext);}

%%

yywrap()
{
	return(1);
}

void alloc_node(int lineno, char *text)
{
   yylval.node = (struct TreeNode *)malloc(sizeof(struct TreeNode));
   int i;
   for(i=0; i<9; i++) yylval.node->child[i] = NULL;
   yylval.node->lineno = lineno;
   yylval.node->isToken = 1;
   strncpy(yylval.node->token, text, 20);
}
