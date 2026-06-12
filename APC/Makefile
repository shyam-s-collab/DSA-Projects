CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = apc
SRC = main.c helpers.c addition.c subtraction.c multiplication.c division.c

all: $(TARGET)

$(TARGET): $(SRC) apc.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	del /Q $(TARGET).exe $(TARGET) 2>NUL || exit 0
