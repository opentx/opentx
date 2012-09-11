CFLAGS=-I../conf -I../inc -g3 -ansi -Wall -O0 
CC=gcc

all : cpi cpo cpa mkdir list rmfile

cpi : cpi.o
	gcc -o cpi cpi.o -L../ -lefsl

cpo : cpo.o
	gcc -o cpo cpo.o -L../ -lefsl

cpa : cpa.o
	gcc -o cpa cpa.o -L../ -lefsl

mkdir : mkdir.o
	gcc -o mkdir mkdir.o -L../ -lefsl

list : list.o
	gcc -o list list.o -L../ -lefsl

rmfile : rmfile.o
	gcc -o rmfile rmfile.o -L../ -lefsl

#cpo2: cpo2.o
#	gcc -o cpo2 cpo2.o -L./lib -lefsl

clean :
	rm -f cpi cpi.o cpo cpo.o cpa.o cpa mkdir.o mkdir list.o list rmfile rmfile.o

