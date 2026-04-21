CC=gcc
CFLAGS=-Wall -Wextra

all: static shared dynamic

static: main.o libsigs.a
	$(CC) main.o -L. -lsigs -o main_static

shared: main.o libsigs.so
	$(CC) main.o -L. -lsigs -Wl,-rpath,. -o main_shared

dynamic: main_dyn.o libsigs.so
	$(CC) main_dyn.o -o main_dynamic -ldl

libsigs.a: sig_default.o sig_ignore.o sig_handle.o sig_mask.o
	ar rcs libsigs.a sig_default.o sig_ignore.o sig_handle.o sig_mask.o

libsigs.so: sig_default.c sig_ignore.c sig_handle.c sig_mask.c
	$(CC) $(CFLAGS) -fPIC -shared -o libsigs.so sig_default.c sig_ignore.c sig_handle.c sig_mask.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

main_dyn.o: main.c
	$(CC) $(CFLAGS) -DDYNAMIC -c main.c -o main_dyn.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.a *.so main_static main_shared main_dynamic