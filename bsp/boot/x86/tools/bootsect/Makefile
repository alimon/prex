include $(SRCDIR)/mk/own.mk
SYMBOL:=	bootsect.sym
TARGET:=	bootsect.bin
OBJS:=		bootsect.o
LDSCRIPT:=	bootsect.ld

include $(SRCDIR)/mk/common.mk

$(TARGET): $(OBJS) $(LDSCRIPT)
	$(call echo-file,LD     ,$@)
	$(LD) $(LDFLAGS) $(OUTPUT_OPTION) $(OBJS)
	$(ASMGEN)
	$(SYMGEN)
	$(OBJCOPY) -O binary -R .note -R .comment -S $@
