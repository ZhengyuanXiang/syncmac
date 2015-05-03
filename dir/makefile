include ../mf_include

PROGS = test_dir.out test_dir_monitor.out
INCLUDE = -I../include
OBJS = dir.o dir_monitor.o

all: $(PROGS) dir.o dir_monitor.o libdir.a

test_dir.o: test_dir.c
test_dir_monitor.o: test_dir_monitor.c

dir.o:dir.c
	$(CC) $(INCLUDE) -c dir.c

dir_monitor.o: dir_monitor.c
	$(CC) $(INCLUDE) -c dir_monitor.c

test_dir.out: test_dir.o dir.o
	$(CC) -o test_dir.out test_dir.o dir.o

test_dir_monitor.out: test_dir_monitor.o dir.o dir_monitor.o
	$(CC) -o test_dir_monitor.out test_dir_monitor.o dir.o dir_monitor.o

libdir.a: $(OBJS)
	$(AR) libdir.a $(OBJS)

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a