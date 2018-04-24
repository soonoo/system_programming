OBJS = server.o util.o
CC = gcc
TARGETS = Server Client
SERVER = Server
CLIENT = Client
FLAGS = -Wall -o

all: $(TARGETS)

$(SERVER): $(OBJS)
	$(CC) $(FLAGS) $@ $^ -lcrypto
	make clean

$(CLIENT): client.c
	$(CC) $(FLAGS) $@ $^
	make clean

clean:
	rm -f *.o
