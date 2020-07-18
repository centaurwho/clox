SDIR := src
IDIR := include

SFILES := $(wildcard $(SDIR)/*.c)
IFILES := $(wildcard $(IDIR)/*.h)

CC=gcc
CFLAGS := -I

ONAME := clox

clox: $(IFILES)
	$(CC) $(CFLAGS) $(IDIR) -o $(ONAME) $(SFILES)

clean:
	rm -rf clox *.out
