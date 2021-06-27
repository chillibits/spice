grammar Spice;

entry: (stmt | functionDef | procedureDef)*;
functionDef: F LESS DATA_TYPE GREATER IDENTIFIER LPAREN paramLst RPAREN LBRACE stmtLst RBRACE;
procedureDef: P IDENTIFIER LPAREN paramLst RPAREN LBRACE stmtLst RBRACE;
forLoop: FOR assignment SEMICOLON topLvlExpr SEMICOLON topLvlExpr LBRACE stmtLst RBRACE;
foreachLoop: FOREACH IDENTIFIER COLON value LBRACE stmtLst RBRACE;
whileLoop: WHILE topLvlExpr LBRACE stmtLst RBRACE;
ifStmt: IF topLvlExpr LBRACE stmtLst RBRACE;

stmtLst: (stmt | forLoop | foreachLoop | whileLoop | ifStmt)*;
paramLst: (declStmt | assignment)? (COMMA (declStmt | assignment))*;
stmt: (declStmt | assignment | functionCall | topLvlExpr | importStmt | returnStmt) SEMICOLON;
declStmt: CONST? DATA_TYPE IDENTIFIER;
functionCall: IDENTIFIER LPAREN paramLst RPAREN;
importStmt: IMPORT STRING;
returnStmt: RETURN topLvlExpr;

topLvlExpr: assignment;
assignment: ((declStmt | IDENTIFIER) (ASSIGN_OP | PLUS_EQUAL | MINUS_EQUAL | MUL_EQUAL | DIV_EQUAL))? ternary;
ternary: logicalOrExpr (QUESTION_MARK logicalOrExpr ':' logicalOrExpr)?;
logicalOrExpr: logicalAndExpr (LOGICAL_OR logicalAndExpr)*;
logicalAndExpr: bitwiseOrExpr (LOGICAL_AND bitwiseOrExpr)*;
bitwiseOrExpr: bitwiseAndExpr (BITWISE_OR bitwiseAndExpr)*;
bitwiseAndExpr: equality (BITWISE_AND equality)*;
equality: relationalExpr ((EQUAL | NOT_EQUAL) relationalExpr)?;
relationalExpr: additiveExpr ((LESS | GREATER | LESS_EQUAL | GREATER_EQUAL) additiveExpr)?;
additiveExpr: multiplicativeExpr ((PLUS | MINUS) multiplicativeExpr)*;
multiplicativeExpr: prefixUnary ((MUL | DIV) prefixUnary)*;
prefixUnary: (NOT | PLUS_PLUS | MINUS_MINUS)? postfixUnary;
postfixUnary: atomicExpr (PLUS_PLUS | MINUS_MINUS)?;
atomicExpr: value | LPAREN assignment RPAREN;
value: STRING | BOOL | INTEGER | DOUBLE | IDENTIFIER | functionCall;

DATA_TYPE: 'double' | 'int' | 'string' | 'bool' | 'dyn';
F: 'f';
P: 'p';
IF: 'if';
FOR: 'for';
FOREACH: 'foreach';
WHILE: 'while';
CONST: 'const';
IMPORT: 'import';
RETURN: 'return';
IDENTIFIER: [_a-zA-Z][_a-zA-Z0-9]*;
INTEGER: '-'* [1-9]+[0-9]* | '0';
DOUBLE: '-'* [0-9]+.[0-9]+;
STRING: '"' (~["\\\r\n] | '\\' (. | EOF))* '"';
BOOL: TRUE | FALSE;
TRUE: 'true';
FALSE: 'false';
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

COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;
WS: [ \t\r\n]+ -> channel(HIDDEN);