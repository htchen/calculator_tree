#ifndef __LEX__
#define __LEX__
#define MAXLEN 256
typedef enum {UNKNOWN, END, INT, ID, ADDSUB, MULDIV, ASSIGN,
LPAREN, RPAREN} TokenSet;
char lexeme[MAXLEN];


extern int match (TokenSet token);
extern void advance(void);
#endif // __LEX__
