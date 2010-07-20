obj-y := system/
obj-y += clock.o debug.o interrupt.o main.o proc.o profile.o \
	 start.o system.o table.o utility.o kernel-syms.o

ccflags-y := -D__KERNEL__

# add built-in in subdir into the list 
obj-y += system/built-in.o

create-builtin := y
