# NTHU I2P2 - Mini Project 1

#### Goal: To implemet a simple calculator for integer arithmetic

```
  >> x = 3
  3
  >> y = -5
  -5
  >> z = 4*x + y*-6
  42
```

There is only one type: integer.
Everything is an expression.
```
  statement   := END | expr END
  expr        := term expr_tail
  expr_tail   := ADDSUB term expr_tail | NIL
  term        := factor term_tail
  term_tail := MULDIV factor term_tail | NIL
  factor      := INT | ADDSUB INT | ADDSUB ID | ID ASSIGN expr | ID | LPAREN expr RPAREN
```  


#### References
1. MIT OCW 6.01 HW1: Calculator — Fall 2011   (google MIT6_01SCS11_hw1.pdf)
2. Wikipedia http://rosettacode.org/wiki/Arithmetic_Evaluator/C
3. “Compiler Design in C”,  Allen Holub

