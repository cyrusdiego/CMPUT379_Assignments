CC = g++
CFLAGS = -std=c++11
OBJDIR = bin/
OBJECTS = shell379.o

.PHONY: clean all

all: shell379

# $(OBJDIR)/%.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

shell379.o: shell379.cpp
	$(CC) $(CFLAGS) -c $< -o $@

shell379: $(OBJECTS)
	$(CC) $(CFLAGS) -o shell379 $(OBJECTS)

# DETECT IF THIS IS FILE IS THERE!!
clean:
	rm *.o shell379 