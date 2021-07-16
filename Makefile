OBJS	= src/hpsdr_debug.o  src/hpsdr_functions.o  src/hpsdr_newprotocol.o  src/hpsdr_sim.o
SOURCE	= src/hpsdr_debug.c  src/hpsdr_functions.c  src/hpsdr_newprotocol.c  src/hpsdr_sim.cc
HEADER	= src/hpsdr_debug.h  src/hpsdr_definitions.h  src/hpsdr_functions.h  src/hpsdr_sim.h
OUT	= hpsdr_sim
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm -lrt -lpthread 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

hpsdrsim.o: src/hpsdr_sim.c
	$(CC) $(FLAGS) hpsdr_sim.c 

newhpsdrsim.o: src/hpsdr_newprotocol.c
	$(CC) $(FLAGS) hpsdr_newprotocol.c 

hpsdr_debug.o: src/hpsdr_debug.c
	$(CC) $(FLAGS) hpsdr_debug.c

hpsdr_functions.o: src/hpsdr_functions.c
	$(CC) $(FLAGS) hpsdr_functions.c

clean:
	rm -f $(OBJS) $(OUT)





