include $(SRCDIR)/mk/own.mk

PROG:=		exec
SRCS:=		main.c exec_execve.c exec_conf.c exec_cap.c \
		exec_elf.c exec_script.c \
		$(SRCDIR)/usr/arch/$(ARCH)/elf_reloc.c

ifeq ($(_DEBUG_),1)
MAP:=		exec.map
DISASM:= 	exec.lst
SYMBOL:= 	exec.sym
endif

include $(SRCDIR)/mk/prog.mk
