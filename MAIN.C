/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "MAIN.H"
/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;
int Error = FALSE;

void main(  )
{
	EOF_flag = FALSE;
	TreeNode * syntaxTree;
  source = fopen("tenp.tny","r");
  //if (source==NULL)
  //{ fprintf(stderr,"File %s not found\n",pgm);
 //   exit(1);
 // }
  listing = fopen("temp.tny","w+");  
  syntaxTree = parse();
  printTree(syntaxTree);
  fclose(source);
  fflush(listing);
}

