/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
{
    START,
    INASSIGN,
    INCOMMENT,
    INNUM, 
    INID,
    DONE,
    INPE,//+=
    INME,//<=
    //INUE//<>
} StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;      /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{
    if (!(linepos < bufsize))
    {
        lineno++;
        if (fgets(lineBuf, BUFLEN - 1, source))
        { //if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++];
        }
        else
        {
            EOF_flag = TRUE;
            return EOF;
        }
    }
    else
        return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{
    if (!EOF_flag)
        linepos--;
}

/* lookup table of reserved words */
static struct
{
    char *str;
    TokenType tok;
} reservedWords[MAXRESERVED] = {{"if", IF},{"enddo", ENDDO},{"for", FOR},{"downto", DOWNTO},{"to",TO},{"while", WHILE},{"do",DO },{"else", ELSE},{"repeat", REPEAT}, {"until", UNTIL}, {"read", READ}, {"write", WRITE},{"endwhile", ENDWHILE}};

/* lookup an identifier to see if it is a reserved word */
static TokenType reservedLookup(char *s)
/* uses linear search */
{
    int i;
    for (i = 0; i < MAXRESERVED; i++)
        if (!strcmp(s, reservedWords[i].str))
            return reservedWords[i].tok;
    return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{ /* index for storing into tokenString */
    int tokenStringIndex = 0;
    /* holds current token to be returned */
    TokenType currentToken;
    /* current state - always begins at START */
    StateType state = START;
    /* flag to indicate save to tokenString */
    int save;
    while (state != DONE)
    {
        int c = getNextChar();
        save = TRUE;
        switch (state)
        {
        case START:
            if (isdigit(c))
                state = INNUM;
            else if (isalpha(c))
                state = INID;
            else if (c == ':')
                state = INASSIGN;
            else if ((c == ' ') || (c == '\t') || (c == '\n')||c=='\r')
                save = FALSE;
            else if (c == '{')
            {
                save = FALSE;
                state = INCOMMENT;
            }
            else
            {
                state = DONE;
                switch (c)
                {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case '=':
                    currentToken = EQ;
                    break;
                case '<':
                    state=INME;
                   // currentToken = LT;
                    break;
                case '+':
                    state = INPE;
                   // currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '^':
                    currentToken = NP;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '/':
                    currentToken = OVER; 
                    break;
                case '(':
                    currentToken = LPAREN;                                              
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case '%':
                    currentToken = MD;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;
        case INCOMMENT:
            save = FALSE;
            if (c == EOF)
            {
                state = DONE;
                currentToken = ENDFILE;
            }
            else if (c == '}')
                state = START;
            break;
        case INASSIGN:
            state = DONE;
            if (c == '=')
                currentToken = ASSIGN;
            else
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break; 
        case INNUM:
            if (!isdigit(c))
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INID:
            if (!isalpha(c))
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break;
         case INPE:
            state = DONE;
            if (c == '=')
                currentToken = PE; 
            else
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                currentToken=PLUS;  
            }
            break; 
        case INME:
            state = DONE;
            if (c == '=')
                currentToken = ME; 
            else if(c=='>')
                currentToken=UE;
            else
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                // currentToken = ERROR;
               currentToken=LT;
            }
            break; 
        case DONE:
        default: /* should never happen */
            fprintf(listing, "Scanner Bug: state= %d\n", state);
            state = DONE;
            currentToken = ERROR;
            break;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN))
            tokenString[tokenStringIndex++] = (char)c;
        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }
    return currentToken;
} /* end getToken */
