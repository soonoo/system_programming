OBJS = server.o util.o
CC = gcc
TARGETS = Server Client
SERVER = Server
CLIENT = Client
FLAGS = -Wall -o

all: $(TARGETS)

$(SERVER): $(OBJS)
	$(CC) $(FLAGS) $@ $^ -lcrypto

$(CLIENT): client.c
	$(CC) $(FLAGS) $@ $^	

clean:
	rm -f *.o
