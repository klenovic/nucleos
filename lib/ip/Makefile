# Makefile for lib/ip.
lib-y := libnucc.a
libnucc.a-obj-y := accept.o bind.o connect.o dhcp_gettag.o dhcp_settag.o domainname.o \
	                 ether_line.o ethera2n.o ethere2a.o etherh2n.o ethern2h.o getdomain.o \
	                 gethnmadr.o gethostent.o gethostname.o getifaddrs.o getnetbyaddr.o \
	                 getnetbyname.o getnetent.o getpeername.o getproto.o getprotoent.o \
	                 getprotoname.o getservent.o getsockname.o getsockopt.o getsrvbyname.o \
	                 getsrvbyport.o hton.o inet_addr.o inet_network.o inet_ntoa.o listen.o \
	                 memcspn.o oneC_sum.o rcmd.o recv.o recvfrom.o res_comp.o res_init.o \
	                 res_mkquery.o res_query.o res_send.o ruserok.o send.o sendto.o servxcheck.o \
	                 sethostent.o setsockopt.o shutdown.o socket.o strcasecmp.o

ccflags-y := -D_MINIX -D_POSIX_SOURCE -DNDEBUG -D__UKERNEL__
