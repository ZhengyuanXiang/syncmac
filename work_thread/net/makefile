include ../../mf_include

PROGS = test_net_opt_server.out test_net_opt_clinet.out

OBJS = net_opt.o
LIBOBJS = ../user/user.o ../../dir/libdir.a

INCLUDE = -I../../include -I../user -I../../dir
LIB = 

all:$(OBJS) $(PROGS)

net_opt.o:net_opt.c
	$(CC) $(INCLUDE) -c net_opt.c -o net_opt.o $(LIB)

test_net_opt_server.out:net_opt.o test_net_opt_server.c
	$(CC) $(INCLUDE) test_net_opt_server.c -o test_net_opt_server.out net_opt.o $(LIBOBJS)

test_net_opt_clinet.out:net_opt.o test_net_opt_clinet.c
	$(CC) $(INCLUDE) test_net_opt_clinet.c -o test_net_opt_clinet.out net_opt.o $(LIBOBJS)

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a