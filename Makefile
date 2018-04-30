SERVER_OBJS = server.o util.o
CC = gcc
TARGETS = Server
SERVER = Server
FLAGS = -Wall -o

$(SERVER): $(SERVER_OBJS)
	$(CC) $(FLAGS) $@ $^ -lcrypto

clean:
	rm -f *.o
