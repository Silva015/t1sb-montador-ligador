;Código fonte do módulo A:
SECTION TEXT
MOD_A: BEGIN
Y: EXTERN
MOD_B: EXTERN
PUBLIC VAL
PUBLIC L1
INPUT Y
LOAD VAL
ADD Y
STORE Y + 2
JMPP MOD_B
L1: STOP
END
SECTION DATA
VAL: CONST 5