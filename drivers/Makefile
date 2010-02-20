# Makefile for all device drivers.
obj-y := amddev/ at_wini/ audio/ bios_wini/ dpeth/ dp8390/ floppy/ fxp/ \
	 lance/ log/ memory/ orinoco/ pci/ printer/ random/ readclock/ \
	 rtl8139/ sb16/ tty/ ti1225/ filter/ rtl8169/ e1000/

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
