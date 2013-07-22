CC=gcc
CFLAGS=-Wall -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=

.PHONY: doc clean

all : mmu

mmu : $(OBJS)
	$(CC) $(LIB) $(OBJS) -o $@

.c.o : $(SRCS)
	$(CC) -g -c $(CFLAGS) $*.c

doc :
	pdflatex *.tex
	rm -rf *.aux *.log *.out

clean : 
	rm -rf *.o *.a mmu

tar :
	tar --exclude=*.o --exclude=*.a --exclude=tests --exclude=steven_lu_lab_3.tgz --exclude=testdata.tar.bz2 -pczf steven_lu_lab_3.tgz *
