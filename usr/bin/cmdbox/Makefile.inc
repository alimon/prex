
PROG:=		cmdbox/cmdbox

DEFS+=		CMDBOX

ifeq ($(_DEBUG_),1)
MAP:= 		cmdbox.map
DISASM:= 	cmdbox.lst
SYMBOL:= 	cmdbox.sym
endif

SRCS=		cmdbox/main.c cmdbox/cmd_conf.c
