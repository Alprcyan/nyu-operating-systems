CC=gcc
CFLAGS=-Wall -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
LIB=

.PHONY: doc clean

all : iosched

iosched : $(OBJS)
	$(CC) $(LIB) $(OBJS) -o $@

.c.o : $(SRCS)
	$(CC) -g -c $(CFLAGS) $*.c

doc :
	pdflatex *.tex
	rm -rf *.aux *.log *.out

clean : 
	rm -rf *.o *.a iosched

tar :
	tar --exclude=*.o --exclude=*.a --exclude=tests --exclude=steven_lu_lab_4.tgz --exclude=testdata.tar.gz -pczf steven_lu_lab_4.tgz *
