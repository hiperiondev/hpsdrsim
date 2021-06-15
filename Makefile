OBJS	= src/hpsdrsim.o src/newhpsdrsim.o
SOURCE	= src/hpsdrsim.c src/newhpsdrsim.c
HEADER	= src/hpsdrsim.h
OUT	= hpsdrsim
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm -lrt -lpthread 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

hpsdrsim.o: src/hpsdrsim.c
	$(CC) $(FLAGS) hpsdrsim.c 

newhpsdrsim.o: src/newhpsdrsim.c
	$(CC) $(FLAGS) newhpsdrsim.c 


clean:
	rm -f $(OBJS) $(OUT)

