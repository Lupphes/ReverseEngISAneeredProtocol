#	
#	Makefile for ISA project
#	Author: Ondřej Sloup (xsloup02)
#

# Makefile for build the whole file
 
# Makefile parameters
CC = g++
CFLAGS = # -Wall -g -Werror -Wpedantic

# Options for makefile 
all: clean build

clean:
	rm -f ./client

build: main.cpp client.hpp handler.hpp requestmsg.hpp errorcodes.hpp
	$(CC) $(CFLAGS) -o client main.cpp client.cpp handler.cpp requestmsg.cpp 
