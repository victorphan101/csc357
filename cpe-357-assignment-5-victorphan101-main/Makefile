CC = gcc
CFLAG = -Wall -pedantic -g -std=c99
HTTPD = httpd
HTTPDOBJS = httpd.o $(NET)
NET = net.h net.c
CLIENT = client
CLIENTOBJS = client.o 

all : $(HTTPD) $(NET) $(CLIENT)

$(HTTPD) : $(HTTPDOBJS)
	$(CC) $(CFLAG) -o $(HTTPD) $(HTTPDOBJS)

$(CLIENT) : $(CLIENTOBJS)
	$(CC) $(CFLAG) -o $(CLIENT) $(CLIENTOBJS)

httpd.o : httpd.c net.h net.c
	$(CC) $(CFLAG) -c httpd.c net.h net.c

client.o : client.c
	$(CC) $(CFLAG) -c client.c 

clean :
	rm *.o $(HTTPD) $(CLIENT) $(NET) core*
