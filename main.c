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

typedef struct _Node {
	char lexeme[MAXLEN];
	TokenSet data;
	int val;
	struct _Node *left, *right;
} BTNode;

void statement(void);
BTNode* expr(void);
BTNode* term(void);
BTNode* factor(void);
int getval(void);
int setval(char*, int);

typedef enum {MISPAREN, NOTNUMID, NOTFOUND, RUNOUT, NAN} ErrorType;
void error(ErrorType errorNum);

/* create a node without any child.*/
BTNode* makeNode(TokenSet tok, const char *lexe)
{
	BTNode *node = (BTNode*) malloc(sizeof(BTNode));
	strcpy(node->lexeme, lexe);
	node->data = tok;
	node->val = 0;
	node->left = NULL;
	node->right = NULL;
	return node;
}

/* clean a tree.*/
void freeTree(BTNode *root)
{
	if (root!=NULL) {
		freeTree(root->left);
		freeTree(root->right);
		free(root);
	}
}

/* print a tree by pre-order. */
void printPrefix(BTNode *root)
{
	if (root != NULL) {
		printf("%s ", root->lexeme);
		printPrefix(root->left);
		printPrefix(root->right);
	}
}

int evaluateTree(BTNode *root)
{
	int retval = 0, lv, rv;
	if (root != NULL) {
		switch (root->data) {
		case ID:
		case INT:
			retval = root->val;
			break;
		case ASSIGN:
		case ADDSUB:
		case MULDIV:
			lv = evaluateTree(root->left);
			rv = evaluateTree(root->right);
			if (strcmp(root->lexeme, "+") == 0)
				retval = lv + rv;
			else if (strcmp(root->lexeme, "-") == 0)
				retval = lv - rv;
			else if (strcmp(root->lexeme, "*") == 0)
				retval = lv * rv;
			else if (strcmp(root->lexeme, "/") == 0) {
				if (rv==0)
					error(NAN);
				else
					retval = lv / rv;
			} else if (strcmp(root->lexeme, "=") == 0)
				retval = setval(root->left->lexeme, rv);
			break;
		default:
			retval = 0;
		}
	}
	return retval;
}

int getval(void)
{
	int i, retval, found;

	if (match(INT)) {
		retval = atoi(getLexeme());
	} else if (match(ID)) {
		i = 0;
		found = 0;
		retval = 0;
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

//  expr        := term expr_tail
//  expr_tail   := ADDSUB term expr_tail | NIL
BTNode* expr(void)
{
	BTNode *retp, *left;
	//int retval;
	retp = left = term();
	while (match(ADDSUB)) {
		retp = makeNode(ADDSUB, getLexeme());
		advance();
		retp->right = term();
		retp->left = left;
		left = retp;
	}
	return retp;
}

//  term        := factor term_tail
//  term_tail := MULDIV factor term_tail | NIL
BTNode* term(void)
{
	BTNode *retp, *left;
	retp = left = factor();

	while (match(MULDIV)) {
		retp = makeNode(MULDIV, getLexeme());
		advance();
		retp->right = factor();
		retp->left = left;
		left = retp;
	}
	return retp;
}

BTNode* factor(void)
{
	BTNode* retp = NULL;
	char tmpstr[MAXLEN];

	if (match(INT)) {
		retp =  makeNode(INT, getLexeme());
		retp->val = getval();
		advance();
	} else if (match(ID)) {
		BTNode* left = makeNode(ID, getLexeme());
		left->val = getval();
		strcpy(tmpstr, getLexeme());
		advance();
		if (match(ASSIGN)) {
			retp = makeNode(ASSIGN, getLexeme());
			advance();
			retp->right = expr();
			retp->left = left;
		} else {
			retp = left;
		}
	} else if (match(ADDSUB)) {
		strcpy(tmpstr, getLexeme());
		advance();
		if (match(ID) || match(INT)) {
			retp = makeNode(ADDSUB, tmpstr);
			if (match(ID))
				retp->right = makeNode(ID, getLexeme());
			else
				retp->right = makeNode(INT, getLexeme());
			retp->right->val = getval();
			retp->left = makeNode(INT, "0");
			retp->left->val = 0;
			advance();
		} else {
			error(NOTNUMID);
		}
	} else if (match(LPAREN)) {
		advance();
		retp = expr();
		if (match(RPAREN)) {
			advance();
		} else {
			error(MISPAREN);
		}
	} else {
		error(NOTNUMID);
	}
	return retp;
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
		break;
	case NAN:
		fprintf(stderr, "Not a number\n");
	}
	exit(0);
}

void statement(void)
{
	BTNode* retp;

	if (match(END)) {
		printf(">> ");
		advance();
	} else {
		retp = expr();
		if (match(END)) {
			printf("%d\n", evaluateTree(retp));
			printPrefix(retp);
			printf("\n");
			freeTree(retp);

			printf(">> ");
			advance();
		}
	}
}

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
