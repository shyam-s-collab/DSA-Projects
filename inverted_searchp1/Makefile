CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -g
TARGET  = inverted_search
SRCDIR  = src
SRCS    = $(SRCDIR)/main.c \
          $(SRCDIR)/utils.c \
          $(SRCDIR)/file_list.c \
          $(SRCDIR)/hashing.c \
          $(SRCDIR)/indexing.c \
          $(SRCDIR)/searching.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/inverted_search.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET) database.txt

run: all
	./$(TARGET)