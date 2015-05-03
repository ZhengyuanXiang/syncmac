include ./mf_include

DIRS = dir work_thread

LIB = -lpthread
INCLUDE = -I./include -I../dir -I./work_thread
LIB_OBJECT = ./dir/libdir.a ./work_thread/work_thread.a

all: submodule

clean:
	for i in $(DIRS); do \
		(cd $$i && make clean); \
	done

submodule:
	for i in $(DIRS); do \
		(cd $$i && make all); \
	done