CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = forky

all: $(TARGET)

$(TARGET): forky.o
	$(CC) $(CFLAGS) -o $(TARGET) forky.o

forky.o: forky.c
	$(CC) $(CFLAGS) -c forky.c

clean:
	rm -f $(TARGET) forky.o

.PHONY: all clean