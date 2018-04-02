OBJS = proxy_cache.o util.o
CC = gcc
TARGET = proxy_cache
FLAGS = -Wall -o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $@ $^ -lcrypto
	make clean

clean:
	rm -f *.o
