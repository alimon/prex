SUBDIR:=	bsp sys usr
SRCDIR:=	$(CURDIR)
export SRCDIR

include $(SRCDIR)/mk/image.mk

prex.i386-pc.img: all prexos
	make -C bsp/boot/x86/tools/bootsect
	mformat -i prex.i386-pc.img -a -C -f 1440 -B bsp/boot/x86/tools/bootsect/bootsect.bin ::
	mcopy -i prex.i386-pc.img prexos ::
	rm bsp/boot/x86/tools/bootsect/bootsect.bin
