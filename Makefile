SERVER_OBJS = server.o util.o
CLIENT_OBJS = client.o util.o
CC = gcc
TARGETS = Server Client
SERVER = Server
CLIENT = Client
FLAGS = -Wall -o

all: $(TARGETS)

$(SERVER): $(SERVER_OBJS)
	$(CC) $(FLAGS) $@ $^ -lcrypto

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(FLAGS) $@ $^ -lcrypto
	make clean

clean:
	rm -f *.o
