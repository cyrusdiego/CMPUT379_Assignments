CC = g++
CFLAGS = -std=c++11
OBJDIR = bin/
OBJECTS = mini_shell.o

.PHONY: clean all

all: mini_shell

# $(OBJDIR)/%.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

mini_shell.o: mini_shell.cpp
	$(CC) $(CFLAGS) -c $< -o $@

mini_shell: $(OBJECTS)
	$(CC) $(CFLAGS) -o mini_shell $(OBJECTS)

# DETECT IF THIS IS FILE IS THERE!!
clean:
	rm *.o mini_shell 