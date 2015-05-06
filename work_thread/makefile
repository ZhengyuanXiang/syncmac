include ../mf_include

PROGS = test_work_thread.out

LIB = -lpthread
INCLUDE = -I../include -I../dir -I../task
LIB_OBJECT = ../dir/libdir.a ../task/libtask.a

OBJS = work_thread.o

all: $(PROGS) work_thread.o libdir.a

work_thread.o: work_thread.c
	$(CC) $(INCLUDE) -c work_thread.c

test_work_thread.out: work_thread.c work_thread.o
	$(CC) $(INCLUDE) test_work_thread.c -o test_work_thread.out work_thread.o $(LIB_OBJECT) $(LIB)

libdir.a: $(OBJS)
	$(AR) libworkthread.a $(OBJS)

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a