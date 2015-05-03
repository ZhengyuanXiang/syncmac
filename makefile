include ./mf_include

DIRS = dir work_thread

PROGS = syncmac.out

LIB = -lpthread
INCLUDE = -I./include -I../dir -I./work_thread
LIB_OBJECT = ./dir/libdir.a ./work_thread/workthread.a

all: submodule

clean:
	for i in $(DIRS); do \
		(cd $$i && make clean); \
	done
	rm -rf $(PROGS)
	rm *.o
	rm -rf *.dSYM
	rm *.a

submodule:
	for i in $(DIRS); do \
		(cd $$i && make all); \
	done