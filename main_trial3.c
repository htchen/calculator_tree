#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "lex.h"
/*
Something like Python

>> y = 2
>> z = 2
>> x = 3*y + 4/(2*z)

*/


/*
the only type: integer
everything is an expression
  statement   := END | expr END
  expr        := term expr_tail
  expr_tail   := ADDSUB term expr_tail | NIL
  term        := factor term_tail
  term_tail := MULDIV factor term_tail | NIL
  factor      := INT | ADDSUB INT | ADDSUB ID | ID ASSIGN expr | ID | LPAREN expr RPAREN
*/



#define TBLSIZE 65535
typedef struct {
    char name[MAXLEN];
    int val;
} Symbol;
Symbol table[TBLSIZE];
int sbcount = 0;

void statement(void);
int expr(void);
int term(void);
int factor(void);


int getval(void);
int setval(char*, int);

typedef enum {MISPAREN, NOTNUMID, NOTFOUND, RUNOUT} ErrorType;
void error(ErrorType errorNum);

int main()
{
    printf(">> ");
    while (1) {
        statement();
    }
    return 0;
}
/*
int main(void)
{
    TokenSet tok;
    tok = getToken();
    while (tok != END) {
        printf("%d: %s\n", tok, getLexeme());
        tok = getToken();
    }
    return 0;
}

*/


int getval(void)
{
    int i, retval, found;

    if (match(INT)) {
        retval = atoi(getLexeme());
    } else if (match(ID)) {
        i = 0; found = 0; retval = 0;
        while (i<sbcount && !found) {
            if (strcmp(getLexeme(), table[i].name)==0) {
                retval = table[i].val;
                found = 1;
                break;
            } else {
                i++;
            }
        }
        if (!found) {
            if (sbcount < TBLSIZE) {
                strcpy(table[sbcount].name, getLexeme());
                table[sbcount].val = 0;
                sbcount++;
            } else {
                error(RUNOUT);
            }
        }
    }
    return retval;
}

int setval(char *str, int val)
{
    int i, retval;
    i = 0;
    while (i<sbcount) {
        if (strcmp(str, table[i].name)==0) {
            table[i].val = val;
            retval = val;
            break;
        } else {
            i++;
        }
    }
    return retval;
}


int expr(void)
{
    int retval;

    retval = term();
    while (match(ADDSUB)) {
        if (strcmp(getLexeme(), "+")==0) {
            advance();
            retval = retval + term();
        } else if (strcmp(getLexeme(), "-")==0) {
            advance();
            retval = retval - term();
        } else {
            // Nil
        }
    }


    return retval;
}

int term(void)
{
    int retval;
    retval = factor();
    while (match(MULDIV)) {
        if (strcmp(getLexeme(), "*")==0) {
            advance();
            retval *= factor();
        } else if (strcmp(getLexeme(), "/")==0) {
            advance();
            retval = retval/factor();
        } else {
            // Nil
        }
    }
    return retval;
}

int factor(void)
{
    int retval = 0;
    char tmpstr[MAXLEN];

    if (match(INT)) {
        retval = getval();
        advance();
    } else if (match(ID)) {
        retval = getval();
        strcpy(tmpstr, getLexeme());
        advance();
        if (match(ASSIGN)) {
            advance();
            retval = expr();
            retval = setval(tmpstr, retval);
        }
    } else if (match(ADDSUB)) {
        strcpy(tmpstr, getLexeme());
        advance();
        if (match(ID) || match(INT)) {
            retval = getval();
            if (strcmp(tmpstr, "-")==0) {
                retval = -retval;
            }
            advance();
        } else {
            error(NOTNUMID);
        }
    }else if (match(LPAREN)) {
        advance();
        retval = expr();
        if (match(RPAREN)) {
            advance();
        } else {
            error(MISPAREN);
        }
    } else {
        error(NOTNUMID);
    }
    return retval;
}

void error(ErrorType errorNum)
{
    switch (errorNum) {
    case MISPAREN:
        fprintf(stderr, "Mismatched parenthesis\n");
        break;
    case NOTNUMID:
        fprintf(stderr, "Number or identifier expected\n");
        break;
    case NOTFOUND:
        fprintf(stderr, "%s not defined\n", getLexeme());
        break;
    case RUNOUT:
        fprintf(stderr, "Out of memory\n");
    }
    exit(0);
}

void statement(void)
{
    int retval;

    if (match(END)) {
        printf(">> ");
        advance();
    } else {
        retval = expr();
        if (match(END)) {
            printf("%d\n", retval);
            printf(">> ");
            advance();
        }
    }
}

