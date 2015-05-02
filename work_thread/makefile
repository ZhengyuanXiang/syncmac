include ../makefile

PROGS = test_work_thread.out

SHARE_LIB = -Lpthread
INCLUDE = -I../include -I../dir
LIB_OBJECT = ../dir/libdir.a

all: $(PROGS) work_thread.o

work_thread.o: work_thread.c
	$(CC) $(INCLUDE) $(SHARE_LIB) -c work_thread.c

test_work_thread.out: work_thread.c work_thread.o
	$(CC) $(INCLUDE) test_work_thread.c -o test_work_thread.out work_thread.o $(LIB_OBJECT)

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a