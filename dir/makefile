include ../makefile

PROGS = test_dir.out

all: $(PROGS) dir.o

test_dir.o: test_dir.c

dir.o:dir.c

test_dir.out: test_dir.o dir.o
	$(CC) -o test_dir.out test_dir.o dir.o

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM