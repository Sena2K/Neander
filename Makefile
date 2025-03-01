CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = memory_program

all: $(TARGET)

$(TARGET): neander.c
	$(CC) $(CFLAGS) -o $(TARGET) neander.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
