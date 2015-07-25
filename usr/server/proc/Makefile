PROG=		proc
SRCS=		main.c proc_hash.c proc_pid.c proc_fork.c proc_exit.c \
		proc_sig.c proc_tty.c

ifeq ($(_DEBUG_),1)
DISASM= 	proc.lst
MAP=	 	proc.map
SYMBOL= 	proc.sym
endif

include $(SRCDIR)/mk/prog.mk
