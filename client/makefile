#	
#	Makefile for ISA project
# 	Reverse-engineering neznámeho protokolu (Ing. Koutenský)
#	Author: Ondřej Sloup (xsloup02)
#	
#	v1.0 – 23.10.2021
#

# Makefile for build the whole file
 
# Makefile parameters
CC = g++
CFLAGS = -std=c++11 #-Wall -g -Werror -Wpedantic

# Options for makefile 
all: clean build

clean:
	rm -f ./client

build: main.cpp client.hpp handler.hpp requestmsg.hpp errorcodes.hpp
	$(CC) $(CFLAGS) -o client main.cpp client.cpp handler.cpp requestmsg.cpp 
