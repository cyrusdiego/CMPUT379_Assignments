CFLAGS = -O -std=c++11
CC = g++
SRC = main.cpp 
OBJ = $(SRC:.cpp = .o)

shell379: main.o shell.o parser.o helpers.o
	$(CC) $(CFLAGS) -o shell379 helpers.o parser.o shell.o main.o
main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp
shell.o: shell.cpp
	$(CC) $(CFLAGS) -c shell.cpp
parser.o: parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp
helpers.o: helpers.cpp
	$(CC) $(CFLAGS) -c helpers.cpp
clean:
	rm -f core *.o
