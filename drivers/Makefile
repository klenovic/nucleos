# Makefile for all device drivers.
obj-y := at_wini/ bios_wini/ floppy/ log/ pci/ random/ \
	 readclock/ tty/ memory/

#subdir-y-flags := not-sort

DRV_PAGE_OFFSET    = 0
DRV_PAGE_ALIGNMENT = 0
DRV_LOAD_PHYSICAL_ADDR = 0

targets := driver_32.lds

# create linker script first
first := $(targets)

CPPFLAGS_driver_32.lds := -D__PAGE_OFFSET__=$(DRV_PAGE_OFFSET) \
			  -D__PAGE_ALIGNMENT__=$(DRV_PAGE_ALIGNMENT) \
			  -D__LOAD_PHYSICAL_ADDR__=$(DRV_LOAD_PHYSICAL_ADDR)
CPPFLAGS_driver_32.lds += -P
CPPFLAGS_driver_32.lds += -D__UKERNEL__
