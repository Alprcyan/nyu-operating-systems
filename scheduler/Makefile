CC=gcc
CFLAGS=-Wall -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=

.PHONY: doc clean

all : scheduler

scheduler : $(OBJS)
	$(CC) $(LIB) $(OBJS) -o $@

.c.o : $(SRCS)
	$(CC) -g -c $(CFLAGS) $*.c

doc :
	pdflatex *.tex
	rm -rf *.aux *.log *.out

clean : 
	rm -rf *.o *.a scheduler

tar :
	tar -pczf steven_lu_lab_2.tgz *
