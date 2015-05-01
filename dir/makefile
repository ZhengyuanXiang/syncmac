include ../makefile

PROGS = test_dir

all: $(PROGS) dir.o

test_dir.o: test_dir.c

dir.o:dir.c

test_dir: test_dir.o dir.o

clean:
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM