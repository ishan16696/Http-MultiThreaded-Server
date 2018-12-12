CC = g++
CFLAGS = -Wall -std=c++14
DEPS = threadPool.h Parse.h send.h server.h
P_FLAGS = -lpthread
OBJ = threadPool.o Parse.o send.o server.o main.o 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(P_FLAGS)

.PHONY: clean

clean:
	rm -rf *.o main

