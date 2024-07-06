CC = gcc
CFLAGS = -g -Wall
HEADERS = headers.h prompt.h constants.h parser.h errors.h input.h shell_config.h command_handler.h warp.h peek.h pastevents.h string_utilities.h proclore.h seek.h path_utilities.h linked_list.h

.DEFAULT_GOAL := main

.PHONY: clean all

all: main

main: main.o parser.o prompt.o errors.o input.o command_handler.o warp.o peek.o pastevents.o string_utilities.o proclore.o seek.o path_utilities.o linked_list.o
	gcc -g $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm a.out *.o