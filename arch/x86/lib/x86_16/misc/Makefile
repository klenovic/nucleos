lib-y := libnucc_16.a
libnucc_16.a-obj-y := getprocessor.o iop_in_blk.o iop_in_str.o iop_in.o \
	       iop_out_blk.o iop_out_str.o iop_out.o

asflags-y := -D__CODE16__ -D__UKERNEL__ -include arch/$(SRCARCH)/include/asm/code16gcc.h
