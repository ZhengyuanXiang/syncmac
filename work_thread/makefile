include ../mf_include

PROGS = test_work_thread.out

LIB = -lpthread
INCLUDE = -I../include -I../dir -I../task
LIB_OBJECT = ../dir/libdir.a ../task/libtask.a

DIRS = user net

OBJS = work_thread.o

all: $(PROGS) work_thread.o libdir.a

submodule:
	for i in $(DIRS); do \
		(cd $$i && make all); \
	done

work_thread.o: submodule work_thread.c
	$(CC) $(INCLUDE) -c work_thread.c

test_work_thread.out: work_thread.c work_thread.o
	$(CC) $(INCLUDE) test_work_thread.c -o test_work_thread.out work_thread.o $(LIB_OBJECT) $(LIB)

libdir.a: $(OBJS)
	$(AR) libworkthread.a $(OBJS)

clean:
	for i in $(DIRS); do \
		(cd $$i && make clean); \
	done
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a