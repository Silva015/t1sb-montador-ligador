;Código fonte do módulo B:
SECTION TEXT
MOD_B: BEGIN
VAL: EXTERN
L1: EXTERN
PUBLIC Y
PUBLIC MOD_B
OUTPUT Y
OUTPUT VAL
OUTPUT Y + 2
JMP L1
END
SECTION DATA
Y: SPACE 3