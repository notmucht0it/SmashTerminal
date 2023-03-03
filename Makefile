CC = gcc
override CFLAGS += -g -Wall #-pedantic-errors -ansi

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.c' -print)
OBJS = $(patsubst %.c, %.o, $(SRCS))

smash: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o smash

clean:
	rm -f $(OBJS) smash