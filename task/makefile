include ../mf_include

OBJS = task.o
INCLUDE = -I../include

all: $(OBJS) libtask.a

task.o: task.c
	$(CC) $(INCLUDE) -c task.c

libtask.a:$(OBJS)
	$(AR) libtask.a $(OBJS)

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a