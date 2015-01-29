CC=g++
CFLAGS=-Wall -std=c++11
LFLAGS=-Wall -lm -pthread

ti: ti.o
	$(CC) $(LFLAGS) ti.o -o ti

ti.o: ti.cpp
	$(CC) $(CFLAGS) -c ti.cpp -o ti.o

all: ti

clean:
	rm ti.o ti

do: clean all
	./ti

.PHONY: all clean do
