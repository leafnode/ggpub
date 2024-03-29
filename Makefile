# Makefile generated by makegen 0.8
CC = gcc
LIBS = -lgadu 
FLAGS = 
DEFINE = 
OUTPUT = ggpub

all: ggpub

ggpub: ggpub.o xmalloc.o dynstuff.o 
	$(CC) $(FLAGS) $(LIBS) -o $(OUTPUT) ggpub.o xmalloc.o dynstuff.o 

ggpub.o: ggpub.c
	$(CC) -c $(FLAGS) $(DEFINE) -o ggpub.o ggpub.c

xmalloc.o: xmalloc.c
	$(CC) -c $(FLAGS) $(DEFINE) -o xmalloc.o xmalloc.c

dynstuff.o: dynstuff.c
	$(CC) -c $(FLAGS) $(DEFINE) -o dynstuff.o dynstuff.c


clean:
	rm *.o
	rm ggpub
