CC = gcc
CAIRO_CFLAGS = $(shell pkg-config --cflags cairo)
CAIRO_LDFLAGS = $(shell pkg-config --libs cairo)
CFLAGS += -O2 -g -Wall -std=gnu99 -DWITH_TIME $(CAIRO_CFLAGS)
LDLIBS = -lm $(CAIRO_LDFLAGS) -lpthread
LDFLAGS =
USE_OPENCL = $(shell cpp -dM config.h | grep USE_OPENCL)
ifneq "$(USE_OPENCL)" ""
	LDLIBS+=$(shell pkg-config --libs OpenCL)
endif

all: intfract

intfract: intfract.o iterate.o iteratel.o iterated.o gcolor.o clinit.o

intfract.o: intfract.c

intfractl.o: intfractl.c

intfractd.o: intfractd.c

iterate.o: iterate.S
	$(CC) $(CFLAGS) -c -o $@ $<

gcolor.o: gcolor.c gcolor.h

clinit.o:

clean:
	rm -f *.o intfract

.PHONY: clean

