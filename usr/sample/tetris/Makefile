TARGET=	tetris
SRCS=	shapes.c tetris.c input.c screen.c

ifeq ($(_DEBUG_),1)
MAP=	tetris.map
DISASM=	tetris.lst
SYMBOL=	tetris.sym
endif

include $(SRCDIR)/mk/prog.mk
