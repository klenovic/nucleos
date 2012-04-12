hostprogs-y := bintoc ehdr2ahdr mkinitrd mkimage

bintoc-obj-y := bintoc.o

ehdr2ahdr-obj-y := ehdr2ahdr.o
HOST_LDFLAGS_ehdr2ahdr := -L$(src)/ -lutils

mkinitrd-obj-y := mkinitrd.o

mkimage-obj-y := mkimage.o

hostlib-y := libutils.a
libutils.a-obj-y := elfparse.o

first := $(hostlib-y)
