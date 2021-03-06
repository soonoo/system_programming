SERVER_OBJS = server.o util.o
CC = gcc
SERVER = proxy_cache
FLAGS = -Wall -o

$(SERVER): $(SERVER_OBJS)
	$(CC) $(FLAGS) $@ $^ -lcrypto -pthread
	make clean

clean:
	rm -f *.o
