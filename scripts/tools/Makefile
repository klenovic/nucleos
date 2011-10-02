hostprogs-y := bintoc ehdr2ahdr mkfs mkinitrd mkimage

HOST_PPFLAGS += -include scripts/tools/include/nucleos/macros.h

bintoc-obj-y := bintoc.o

ehdr2ahdr-obj-y := ehdr2ahdr.o
HOST_LDFLAGS_ehdr2ahdr := -L$(src)/ -lutils

mkfs-obj-y := mkfs.o
HOST_LDFLAGS_mkfs := -L$(src)/ -lutils

mkinitrd-obj-y := mkinitrd.o

mkimage-obj-y := mkimage.o

hostlib-y := libutils.a
libutils.a-obj-y := elfparse.o rawfs.o

first := $(hostlib-y)
