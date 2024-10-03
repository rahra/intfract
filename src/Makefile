CC = gcc
CAIRO_CFLAGS = $(shell pkg-config --cflags cairo)
CAIRO_LDFLAGS = $(shell pkg-config --libs cairo)
CFLAGS = -O2 -g -Wall -std=gnu99 -DWITH_TIME $(CAIRO_CFLAGS) -DWITH_THREADS
ASFLAGS =
LDLIBS = -lm $(CAIRO_LDFLAGS) -lpthread
LDFLAGS = -z noexecstack

all: intfract

intfract: intfract.o iterate.o iteratel.o iterated.o

intfract.o: intfract.c

intfractl.o: intfractl.c

intfractd.o: intfractd.c

iterate.o: iterate.S

clean:
	rm -f *.o intfract

.PHONY: clean

