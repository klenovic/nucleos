hostprogs-y := bintoc ehdr2ahdr edparams installboot mkfs mkinitrd partition

bintoc-obj-y := bintoc.o

edparams-obj-y := edparams.o
HOST_CFLAGS_edparams.o := -DUNIX
HOST_LDFLAGS_edparams := -L$(src)/ -lutils

ehdr2ahdr-obj-y := ehdr2ahdr.o
HOST_LDFLAGS_ehdr2ahdr := -L$(src)/ -lutils

installboot-obj-y := installboot.o
HOST_CFLAGS_installboot.o := -D_EM_WSIZE=4
HOST_LDFLAGS_installboot := -L$(src)/ -lutils

mkfs-obj-y := mkfs.o
HOST_CFLAGS_mkfs.o := -D_EM_WSIZE=4
HOST_LDFLAGS_mkfs := -L$(src)/ -lutils

mkinitrd-obj-y := mkinitrd.o

partition-obj-y := partition.o

hostlib-y := libutils.a
libutils.a-obj-y := elfparse.o rawfs.o
HOST_CFLAGS_rawfs.o := -D_EM_WSIZE=4 -DUNIX

first := $(hostlib-y)
