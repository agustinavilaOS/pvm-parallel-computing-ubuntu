CC = gcc
CFLAGS = -Wall -g -I/usr/include/pvm3
LDFLAGS = -L/usr/lib -lpvm3

TARGETS = master slave

all: $(TARGETS)

master: master.c
        $(CC) $(CFLAGS) master.c -o master $(LDFLAGS)

slave: slave.c
        $(CC) $(CFLAGS) slave.c -o slave $(LDFLAGS)

clean:
        rm -f $(TARGETS) *.o
