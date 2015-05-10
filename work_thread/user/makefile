include ../../mf_include

OBJS = usr.o

INCLUDE = -I../../include -I../../dir
LIB_OBJECT = ../../dir/libdir.a

all:$(OBJS)

usr.o:user.c
	$(CC) $(INCLUDE) -c user.c $(LIB_OBJECT)

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a