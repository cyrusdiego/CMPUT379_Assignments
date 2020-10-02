# CC = g++
# CFLAGS = -std=c++11
# OBJDIR = bin/
# OBJECTS = shell379.o

CFLAGS = -O -std=c++11
CC = g++
SRC = main.cpp 
OBJ = $(SRC:.cpp = .o)
OBJDIR = bin/

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

# shell379: $(OBJ)
# 	$(CC) $(CFLAGS) -o shell379 $(OBJ)
# clean:
# 	rm -f core *.o
# .PHONY: clean all

# all: shell379

# # $(OBJDIR)/%.o: %.c
# # 	$(CC) $(CFLAGS) -c $< -o $@

# shell379.o: shell379.cpp
# 	$(CC) $(CFLAGS) -c $< -o $@

# shell379: $(OBJECTS)
# 	$(CC) $(CFLAGS) -o shell379 $(OBJECTS)

# # DETECT IF THIS IS FILE IS THERE!!
# clean:
# 	rm *.o shell379 