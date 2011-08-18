lib-y := libnucc.a

libnucc.a-obj-y += memcpy_$(BITS).o

ifeq ($(CONFIG_X86_32),y)
        libnucc.a-obj-y += strstr_32.o
        libnucc.a-obj-y += string_32.o
endif

ccflags-y := -D__UKERNEL__