%{
#include <stdio.h>
#include "tree.h"
extern char yytext[];//
extern int yylineno;
extern int yycolumn;
int is_error;
%}
%locations

%union {
    struct TreeNode* node;
}

/* declared tokens */
%token <node>  SEMI COMMA ASSIGNOP RELOP
%token <node>  AND OR
%token <node>  PLUS MINUS STAR  DIV 
%token <node>  DOT NOT 
%token <node>  LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE ID
%token <node>  INT
%token <node>  FLOAT 
%token <node>  TYPE

%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  PLUS MINUS
%left   STAR DIV
%right  NOT 
%left  LB RB LP RP DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec  VarList ParamDec
%type <node> CompSt StmtList Stmt DefList Def DecList Dec Exp Args 

%%

Program			
	: ExtDefList {
                      $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                      strncpy($$->token, "Program", 20);
	              $$->parent = NULL;
                      Insert($$, $1);
                      if(is_error != 1){
                          //PrintTree($$);
                          init_symbol_table();
                          Traverse($$);
                          FreeTree($$); 
                          delete_symbol_table();
                      }            
                     }
	;

//
ExtDefList
	: ExtDef ExtDefList{
                            $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                            strncpy($$->token, "ExtDefList", 20); 
                            Insert($$, $1);
                            Insert($$, $2);       
                           }
	|{ $$ = NULL; }
	;

ExtDef
	: Specifier ExtDecList SEMI{
                                    $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                    strncpy($$->token, "ExtDef", 20);                                      
                                    Insert($$, $1);
                                    Insert($$, $2);
                                    Insert($$, $3);
                                  }
           
	| Specifier SEMI{
                         $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                         strncpy($$->token, "ExtDef", 20);                                       
                         Insert($$, $1);
                         Insert($$, $2);
                        }
	| Specifier FunDec CompSt{
                                  $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                  strncpy($$->token, "ExtDef", 20); 
                                  Insert($$, $1);
                                  Insert($$, $2);
                                  Insert($$, $3);
                                 }
        | Specifier FunDec error SEMI { is_error = 1; printf("Incomplete definition of function\" %s \"\n", $2->child[0]->ID); yyerrok; } 
	;

ExtDecList
	:	VarDec{
                        $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                        strncpy($$->token, "ExtDecList", 20);
                        Insert($$, $1);
                      }
	|	VarDec COMMA ExtDecList{
                                        $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                        strncpy($$->token, "ExtDecList", 20);
                                        Insert($$, $1);
                                        Insert($$, $2);
                                        Insert($$, $3);
                                       }
	;

Specifier
	:	TYPE{
                      $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                      strncpy($$->token, "Specifier", 20);
                      Insert($$, $1);
                    }
	| StructSpecifier{
                           $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                           strncpy($$->token, "Specifier", 20);
                           Insert($$, $1);
                         }
	;

StructSpecifier
	: STRUCT OptTag LC DefList RC{
                                    $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                    strncpy($$->token, "StructSpecifier", 20);
                                    Insert($$, $1);
                                    Insert($$, $2);
                                    Insert($$, $3);
                                    Insert($$, $4);
                                    Insert($$, $5);
                                     }
	| STRUCT Tag{
                     $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                     strncpy($$->token, "StructSpecifier", 20);
                     Insert($$, $1);
                     Insert($$, $2);                  
                    }
	;

OptTag
	: ID{
             $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
             strncpy($$->token, "OptTag", 20);
             Insert($$, $1);
            }
	|{ $$ = NULL; }
	;

Tag
	: ID{
              $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
              strncpy($$->token, "Tag", 20);
              Insert($$, $1);
            }
	;

VarDec
	: ID{
             $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
             strncpy($$->token, "VarDec", 20);
             Insert($$, $1);
            }
	| VarDec LB INT RB{
                           $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                           strncpy($$->token, "VarDec", 20);
                           Insert($$, $1);
                           Insert($$, $2);
                           Insert($$, $3);
                           Insert($$, $4);
                          }
	;

FunDec
	: ID LP VarList RP{
                           $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                           strncpy($$->token, "FunDec", 20);
                           Insert($$, $1);
                           Insert($$, $2);
                           Insert($$, $3);
                           Insert($$, $4);
                          }
	| ID LP RP{
                    $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                    strncpy($$->token, "FunDec", 20);
                    Insert($$, $1);
                    Insert($$, $2);
                    Insert($$, $3);
                  }
	;

VarList
	: ParamDec COMMA VarList{
                                 $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                 strncpy($$->token, "VarList", 20);
                                 Insert($$, $1);
                                 Insert($$, $2);
                                 Insert($$, $3);
                                }
	| ParamDec{
                   $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                   strncpy($$->token, "VarList", 20);
                   Insert($$, $1);
                  }
	;

ParamDec
	: Specifier VarDec{
                           $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                           strncpy($$->token, "ParamDec", 20);
                           Insert($$, $1);
                           Insert($$, $2);
                          }
	;

CompSt
	: LC DefList StmtList RC{
                                 $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                 strncpy($$->token, "CompSt", 20);
                                 Insert($$, $1);
                                 Insert($$, $2);
                                 Insert($$, $3);
                                 Insert($$, $4);
                                }
        | error RC { is_error = 1; }
	;

StmtList
	: Stmt StmtList{
                        $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                        strncpy($$->token, "StmtList", 20);
                        Insert($$, $1);
                        Insert($$, $2);
                       }
	|  { $$ = NULL;}
	;

Stmt
	: Exp SEMI{
                   $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                   strncpy($$->token, "Stmt", 20);
                   Insert($$, $1);
                   Insert($$, $2);
                  }
	| CompSt{
                 $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                 strncpy($$->token, "Stmt", 20);
                 Insert($$, $1);
                }
	| RETURN Exp SEMI{
                          $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                          strncpy($$->token, "Stmt", 20);
                          Insert($$, $1);
                          Insert($$, $2);
                          Insert($$, $3);
                         }
	| IF LP Exp RP Stmt{
                            $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                            strncpy($$->token, "Stmt", 20);
                            Insert($$, $1);
                            Insert($$, $2);
                            Insert($$, $3);
                            Insert($$, $4);
                            Insert($$, $5);
                           }
	| IF LP Exp RP Stmt ELSE Stmt{
                                     $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                     strncpy($$->token, "Stmt", 20);
                                     Insert($$, $1);
                                     Insert($$, $2);
                                     Insert($$, $3);
                                     Insert($$, $4);
                                     Insert($$, $5);
                                     Insert($$, $6);
                                     Insert($$, $7);
                                     }
	| WHILE LP Exp RP Stmt{
                               $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                               strncpy($$->token, "Stmt", 20);
                               Insert($$, $1);
                               Insert($$, $2);
                               Insert($$, $3);
                               Insert($$, $4);
                               Insert($$, $5);
                              }
        | error SEMI{ is_error = 1; }
	;

DefList
	: Def DefList{
                      $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                      strncpy($$->token, "DefList", 20);
                      Insert($$, $1);
                      Insert($$, $2);
                     } 
	|{ $$ = NULL; }        
	;

Def
	: Specifier DecList SEMI{
                                 $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                                 strncpy($$->token, "Def", 20);
                                 Insert($$, $1);
                                 Insert($$, $2);
                                 Insert($$, $3);
                                }
         |error SEMI { is_error = 1; yyerrok; }
         |error TYPE { is_error = 1; yyerrok; }
	;

DecList
	: Dec{
              $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
              strncpy($$->token, "DecList", 20);
              Insert($$, $1);
             } 
	| Dec COMMA DecList{
                            $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                            strncpy($$->token, "DecList", 20);
                            Insert($$, $1);
                            Insert($$, $2);
                            Insert($$, $3);
                           } 
	;

Dec
	: VarDec{
                 $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                 strncpy($$->token, "Dec", 20);
                 Insert($$, $1);
                } 
	| VarDec ASSIGNOP Exp{
                              $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                              strncpy($$->token, "Dec", 20);
                              Insert($$, $1);
                              Insert($$, $2);
                              Insert($$, $3);
                             } 
	;

Exp
	: Exp ASSIGNOP Exp{
                           $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                           strncpy($$->token, "Exp", 20);
                           Insert($$, $1);
                           Insert($$, $2);
                           Insert($$, $3);
                          } 
	| Exp AND Exp{
                      $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                      strncpy($$->token, "Exp", 20);
                      Insert($$, $1);
                      Insert($$, $2);
                      Insert($$, $3);
                     } 
	| Exp OR Exp{
                     $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                     strncpy($$->token, "Exp", 20);
                     Insert($$, $1);
                     Insert($$, $2);
                     Insert($$, $3);
                    } 
	| Exp RELOP Exp{
                        $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                        strncpy($$->token, "Exp", 20);
                        Insert($$, $1);
                        Insert($$, $2);
                        Insert($$, $3);
                     } 
	| Exp PLUS Exp{
                       $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                       strncpy($$->token, "Exp", 20);
                       Insert($$, $1);
                       Insert($$, $2);
                       Insert($$, $3);
                      } 
	| Exp MINUS Exp{
                        $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                        strncpy($$->token, "Exp", 20);
                        Insert($$, $1);
                        Insert($$, $2);
                        Insert($$, $3);
                       } 
	| Exp STAR Exp{
                      $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                      strncpy($$->token, "Exp", 20);
                      Insert($$, $1);
                      Insert($$, $2);
                      Insert($$, $3);
                     } 
	| Exp DIV Exp{
                      $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                      strncpy($$->token, "Exp", 20);
                      Insert($$, $1);
                      Insert($$, $2);
                      Insert($$, $3);
                     } 
	| LP Exp RP{
                    $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                    strncpy($$->token, "Exp", 20);
                    Insert($$, $1);
                    Insert($$, $2);
                    Insert($$, $3);
                   } 
	| MINUS Exp{
                    $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                    strncpy($$->token, "Exp", 20);
                    Insert($$, $1);
                    Insert($$, $2);
                   } 
	| NOT Exp{
                   $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                   strncpy($$->token, "Exp", 20);
                   Insert($$, $1);
                   Insert($$, $2);
                 } 
	| ID LP Args RP{
                        $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                        strncpy($$->token, "Exp", 20);
                        Insert($$, $1);
                        Insert($$, $2);
                        Insert($$, $3);
                        Insert($$, $4);
                       } 
	| ID LP RP{
                   $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                   strncpy($$->token, "Exp", 20);
                   Insert($$, $1);
                   Insert($$, $2);
                   Insert($$, $3);
                  }
	| Exp LB Exp RB{
                        $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                        strncpy($$->token, "Exp", 20);
                        Insert($$, $1);
                        Insert($$, $2);
                        Insert($$, $3);
                        Insert($$, $4);
                       } 
	| Exp DOT ID{
                     $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                     strncpy($$->token, "Exp", 20);
                     Insert($$, $1);
                     Insert($$, $2);
                     Insert($$, $3);
                    }
	| ID{
              $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
              strncpy($$->token, "Exp", 20);
              Insert($$, $1);
              $$->lineno = $1->lineno;
            } 
	| INT{
               $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
               strncpy($$->token, "Exp", 20);
               Insert($$, $1);
               $$->lineno = $1->lineno;
             } 
	| FLOAT{
                $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                strncpy($$->token, "Exp", 20);
                Insert($$, $1);
                $$->lineno = $1->lineno;
               } 
        | Exp LB error RB { is_error = 1; }
        | LB error RB     { is_error = 1; }
        | error LP        { is_error = 1; }
	;

Args
	: Exp COMMA Args{
                         $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
                         strncpy($$->token, "Args", 20);
                         Insert($$, $1);
                         Insert($$, $2);
                         Insert($$, $3);
                        }
	| Exp{
              $$ = (struct TreeNode *)malloc(sizeof(struct TreeNode));
              strncpy($$->token, "Args", 20);
              Insert($$, $1);
             } 
	;

%%

main(int argc, char **argv)
{
	if(argc <= 1)
		return 1;
	FILE *f = fopen(argv[1],"r");
	if(!f){
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yylineno = 1;
	yyparse();
	return 0;
}

yyerror(char *s)
{
	fflush(stdout);
	printf("Error type B at line %d column %d :", yylineno, yycolumn);
}


