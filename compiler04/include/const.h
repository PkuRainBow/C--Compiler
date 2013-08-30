#ifndef __CONST_H__
#define __CONST_H__


typedef enum {false, true} bool;
/* size of a integer */
#define INT_SIZE    4

/* High-Level Definitions */
#define PROGRAM__EXTDEFLIST				                0x0000

#define EXTDEFLIST__EXTDEF_EXTDEFLIST			        0x0010

#define EXTDEF__SPECIFIER_EXTDECLIST_SEMI		        0x0020
#define EXTDEF__SPECIFIER_SEMI				            0x0021
#define EXTDEF__SPECIFIER_FUNDEC_COMPST			        0x0022

#define EXTDECLIST__VARDEC				                0x0030
#define EXTDECLIST__VARDEC_COMMA_EXTDECLIST		        0x0031

/* Specifiers */
#define SPECIFIER__TYPE					                0x0100
#define SPECIFIER__STRUCTSPECIFIER			            0x0101

#define STRUCTSPECIFIER__STRUCT_OPTTAG_LC_DEFLIST_RC	0x0110
#define STRUCTSPECIFIER__STRUCT_TAG			            0x0111

#define OPTTAG__ID					                    0x0120

#define TAG__ID						                    0x0130

/* Declarators */
#define VARDEC__ID					                    0x0200
#define VARDEC__VARDEC_LB_INT_RB			            0x0201

#define FUNDEC__ID_LP_VARLIST_RP			            0x0210
#define FUNDEC__ID_LP_RP				                0x0211

#define VARLIST__PARAMDEC_COMMA_VARLIST			        0x0220
#define VARLIST__PARAMDEC				                0x0221

#define PARAMDEC__SPECIFIER_VARDEC			            0x0230

/* Statements */
#define COMPST__LC_DEFLIST_STMTLIST_RC			        0x0300

#define STMTLIST__STMT_STMTLIST				            0x0310

#define STMT__EXP_SEMI					                0x0320
#define STMT__COMPST					                0x0321
#define STMT__RETURN_EXP_SEMI				            0x0322
#define STMT__IF_LP_EXP_RP_STMT				            0x0323
#define STMT__IF_LP_EXP_RP_STMT_ELSE_STMT		        0x0324
#define STMT__WHILE_LP_EXP_RP_STMT			            0x0325

/* Local Definitions */
#define DEFLIST__DEF_DEFLIST				            0x0400

#define DEF__SPECIFIER_DECLIST_SEMI			            0x0410

#define DECLIST__DEC					                0x0420
#define DECLIST__DEC_COMMA_DECLIST			            0x0421

#define DEC__VARDEC					                    0x0430
#define DEC__VARDEC_ASSIGNOP_EXP			            0x0431

/* Expressions */
#define EXP__EXP_ASSIGNOP_EXP			0x0500
#define EXP__EXP_AND_EXP				0x0501
#define EXP__EXP_OR_EXP					0x0502
#define EXP__EXP_RELOP_EXP				0x0503
#define EXP__EXP_PLUS_EXP				0x0504
#define EXP__EXP_MINUS_EXP				0x0505
#define EXP__EXP_STAR_EXP				0x0506
#define EXP__EXP_DIV_EXP				0x0507
#define EXP__LP_EXP_RP					0x0508
#define EXP__MINUS_EXP					0x0509
#define EXP__NOT_EXP					0x050A
#define EXP__ID_LP_ARGS_RP				0x050B
#define EXP__ID_LP_RP					0x050C
#define EXP__EXP_LB_EXP_RB				0x050D
#define EXP__EXP_DOT_ID					0x050E
#define EXP__ID						    0x050F

#define EXP__INT					    0x0510
#define EXP__FLOAT					    0x0511

#define ARGS__EXP_COMMA_ARGS			0x0520
#define ARGS__EXP					    0x0521

#endif /* __CONST_H__ */
