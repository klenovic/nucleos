lib-y := libnucc_16.a
libnucc_16.a-obj-y := memcpy.o memset.o strcat.o strchr.o strcmp.o \
	       strcpy.o strlen.o strncmp.o strncpy.o

asflags-y := -D__CODE16__ -D_EM_WSIZE=4 -include arch/$(SRCARCH)/include/asm/code16gcc.h
