CC = gcc
CFLAGS = -Wall -pedantic -g -std=c99
ORIG = original
COMMONOBJS = main.o util.o
ORIGOBJS = original.o $(COMMONOBJS)
BEST = best
BESTOBJS = best.o $(COMMONOBJS)
WORST = worst
WORSTOBJS = worst.o $(COMMONOBJS)
ALT = alternative
ALTOBJS = alternative.o $(COMMONOBJS)

all : $(ORIG) $(BEST) $(WORST) $(ALT)

$(ORIG) : $(ORIGOBJS)
	$(CC) $(CFLAGS) -o $(ORIG) $(ORIGOBJS)

$(BEST) : $(BESTOBJS)
	$(CC) $(CFLAGS) -o $(BEST) $(BESTOBJS)

$(WORST) : $(WORSTOBJS)
	$(CC) $(CFLAGS) -o $(WORST) $(WORSTOBJS)

$(ALT) : $(ALTOBJS)
	$(CC) $(CFLAGS) -o $(ALT) $(ALTOBJS)

original.o : original.c mem.h
	$(CC) $(CFLAGS) -c original.c

best.o : best.c mem.h
	$(CC) $(CFLAGS) -c best.c

worst.o : worst.c mem.h
	$(CC) $(CFLAGS) -c worst.c

alternative.o : alternative.c mem.h
	$(CC) $(CFLAGS) -c alternative.c

main.o : main.c mem.h util.h
	$(CC) $(CFLAGS) -c main.c

util.o : util.c util.h mem.h
	$(CC) $(CFLAGS) -c util.c

clean :
	rm *.o $(ORIG) $(BEST) $(WORST) $(ALT) core*
