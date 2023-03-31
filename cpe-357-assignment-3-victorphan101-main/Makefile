CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -D_BSD_SOURCE
MAIN = tree.c
OBJS = array_list.c array_list.h

all : $(MAIN)

$(MAIN) : $(OBJS) tree.c
        $(CC) -o tree $(MAIN) $(OBJS)

tree.c : tree.c array_list.c array_list.h
        $(CC) $(CFLAGS) $(MAIN) $(OBJS) -g 

clean :
        rm *.o $(MAIN) core

