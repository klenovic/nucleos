lib-y := libnucc.a
libnucc.a-obj-y := alloca.o _cpuid.o _cpufeature.o get_bp.o \
	           getprocessor.o iop.o oneC_sum.o

asflags-y := -O -D_MINIX -D_POSIX_SOURCE
ccflags-y := -D__UKERNEL__
