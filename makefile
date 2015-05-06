include ./mf_include

DIRS = task dir work_thread

PROGS = syncmac_clinet.out

LIB = -lpthread
INCLUDE = -I./include -I./dir -I./work_thread
LIB_OBJECT = ./dir/libdir.a ./work_thread/libworkthread.a ./task/libtask.a

all: $(PROGS)

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

syncmac_clinet.out: submodule
	$(CC) $(INCLUDE) syncmac_clinet.c -o syncmac_clinet.out $(LIB_OBJECT) $(LIB)