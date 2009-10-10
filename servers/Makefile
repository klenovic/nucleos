# Makefile for all system servers.
#
obj-y := ds/ inet/ init/ ipc/ is/ mfs/ pm/ rs/ vfs/ vm/
# iso9660fs/

SERV_PAGE_OFFSET    = 0
SERV_PAGE_ALIGNMENT = 0
SERV_LOAD_PHYSICAL_ADDR = 0

targets := server_32.lds

# create linker script first
first = $(targets)

CPPFLAGS_server_32.lds := -D__PAGE_OFFSET__=$(SERV_PAGE_OFFSET) \
			  -D__PAGE_ALIGNMENT__=$(SERV_PAGE_ALIGNMENT) \
			  -D__LOAD_PHYSICAL_ADDR__=$(SERV_LOAD_PHYSICAL_ADDR)
CPPFLAGS_server_32.lds += -P
CPPFLAGS_server_32.lds += -D__UKERNEL__
