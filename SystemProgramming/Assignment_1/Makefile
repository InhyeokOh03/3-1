CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = main
SOURCES = main.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET)