# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -g #-Werror -Wpedantic
 
# ****************************************************
# Targets needed to bring the executable up to date
 
client: main.cpp client.hpp handler.hpp requestmsg.hpp errorcodes.hpp
	$(CC) $(CFLAGS) -o ./../build/client  main.cpp client.cpp handler.cpp requestmsg.cpp 
 