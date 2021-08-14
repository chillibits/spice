grammar Spice;

entry: (stmt | mainFunctionDef | functionDef | procedureDef)*;
mainFunctionDef: F LESS TYPE_INT GREATER MAIN LPAREN paramLstDef? RPAREN LBRACE stmtLst RBRACE;
functionDef: F LESS dataType GREATER IDENTIFIER LPAREN paramLstDef? RPAREN LBRACE stmtLst RBRACE;
procedureDef: P IDENTIFIER LPAREN paramLstDef? RPAREN LBRACE stmtLst RBRACE;
forLoop: FOR assignment SEMICOLON assignment SEMICOLON assignment LBRACE stmtLst RBRACE;
//foreachLoop: FOREACH IDENTIFIER COLON assignment LBRACE stmtLst RBRACE;
whileLoop: WHILE assignment LBRACE stmtLst RBRACE;
ifStmt: IF assignment LBRACE stmtLst RBRACE;

stmtLst: (stmt | forLoop | /*foreachLoop |*/ whileLoop | ifStmt)*;
paramLstDef: (declStmt | assignment) (COMMA (declStmt | assignment))*;
paramLstCall: assignment (COMMA assignment)*;
stmt: (declStmt | assignment | functionCall | importStmt | returnStmt | breakStmt | continueStmt | printfStmt) SEMICOLON;
declStmt: CONST? dataType IDENTIFIER;
functionCall: (IDENTIFIER DOT)* IDENTIFIER LPAREN paramLstCall? RPAREN;
importStmt: IMPORT STRING AS IDENTIFIER;
returnStmt: RETURN assignment;
breakStmt: BREAK INTEGER?;
continueStmt: CONTINUE INTEGER?;
printfStmt: PRINTF LPAREN STRING (COMMA assignment)* RPAREN;

assignment: ((declStmt | IDENTIFIER) (ASSIGN_OP | PLUS_EQUAL | MINUS_EQUAL | MUL_EQUAL | DIV_EQUAL))? ternary;
ternary: logicalOrExpr (QUESTION_MARK logicalOrExpr ':' logicalOrExpr)?;
logicalOrExpr: logicalAndExpr (LOGICAL_OR logicalAndExpr)*;
logicalAndExpr: bitwiseOrExpr (LOGICAL_AND bitwiseOrExpr)*;
bitwiseOrExpr: bitwiseAndExpr (BITWISE_OR bitwiseAndExpr)*;
bitwiseAndExpr: equalityExpr (BITWISE_AND equalityExpr)*;
equalityExpr: relationalExpr ((EQUAL | NOT_EQUAL) relationalExpr)?;
relationalExpr: additiveExpr ((LESS | GREATER | LESS_EQUAL | GREATER_EQUAL) additiveExpr)?;
additiveExpr: multiplicativeExpr ((PLUS | MINUS) multiplicativeExpr)*;
multiplicativeExpr: prefixUnary ((MUL | DIV) prefixUnary)*;
prefixUnary: (NOT | PLUS_PLUS | MINUS_MINUS)? postfixUnary;
postfixUnary: atomicExpr (PLUS_PLUS | MINUS_MINUS)?;
atomicExpr: value | LPAREN assignment RPAREN;
value: STRING | TRUE | FALSE | INTEGER | DOUBLE | IDENTIFIER | functionCall;
dataType: TYPE_DOUBLE | TYPE_INT | TYPE_STRING | TYPE_BOOL | TYPE_DYN;

TYPE_DOUBLE: 'double';
TYPE_INT: 'int';
TYPE_STRING: 'string';
TYPE_BOOL: 'bool';
TYPE_DYN: 'dyn';
F: 'f';
P: 'p';
IF: 'if';
FOR: 'for';
//FOREACH: 'foreach';
WHILE: 'while';
CONST: 'const';
IMPORT: 'import';
BREAK: 'break';
CONTINUE: 'continue';
RETURN: 'return';
AS: 'as';
MAIN: 'main';
PRINTF: 'printf';
TRUE: 'true';
FALSE: 'false';
STRING: '"' (~["\\\r\n] | '\\' (. | EOF))* '"';
INTEGER: '-'* [1-9]+[0-9]* | '0';
DOUBLE: '-'* [0-9]+.[0-9]+;
IDENTIFIER: [_a-zA-Z][_a-zA-Z0-9]*;
LBRACE: '{';
RBRACE: '}';
LPAREN: '(';
RPAREN: ')';
LBRACKET: '[';
RBRACKET: ']';
LOGICAL_OR: '||';
LOGICAL_AND: '&&';
BITWISE_OR: '|';
BITWISE_AND: '&';
NOT: '!';
PLUS_PLUS: '++';
MINUS_MINUS: '--';
PLUS_EQUAL: '+=';
MINUS_EQUAL: '-=';
MUL_EQUAL: '*=';
DIV_EQUAL: '/=';
PLUS: '+';
MINUS: '-';
MUL: '*';
DIV: '/';
GREATER: '>';
LESS: '<';
GREATER_EQUAL: '>=';
LESS_EQUAL: '<=';
EQUAL: '==';
NOT_EQUAL: '!=';
ASSIGN_OP: '=';
QUESTION_MARK: '?';
SEMICOLON: ';';
COLON: ':';
COMMA: ',';
DOT: '.';

COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;
WS: [ \t\r\n]+ -> channel(HIDDEN);