# Makefile for VM server
obj-y := alloc.o break.o exec.o exit.o fork.o main.o mmap.o signal.o \
	 slaballoc.o region.o pagefaults.o utility.o vfs.o

ccflags-y := -D__UKERNEL__
ccflags-$(CONFIG_CPROFILE) += $(CPROFILE)

create-builtin := y
