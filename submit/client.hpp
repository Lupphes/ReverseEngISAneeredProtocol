/**
 * ISA Projekt
 * Ondřej Sloup – (xsloup02)
 * Reverse-engineering neznámeho protokolu (Ing. Koutenský)
 * 
 * client.hpp
 * v1.0 – 23.10.2021
 */

#pragma once

#include "errorcodes.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <regex>
#include <getopt.h>
#include <netdb.h>
#include <iostream>

using namespace std;

/**
 * Holds value for printing help message
 */
void printHelpMessage();

/**
 * Parsing arguments so they can be processed and validated
 *
 * This function returns a port and an address in their own variable and
 * command inside a vector
 *
 * @param[in] int number of arguments
 * @param[in] char** list of the arguments
 * @param[out] int vector of command variables
 * @param[out] string address set by user where the server should be
 * @param[out] int port set by user where the server should be
 * @return int which signalizes a return code (0 is a success)
 */
int parseArgs(int argc, char** argv, int &args_processed, string &address, int &port);

/**
 * Creates a socket and connects to the server via set port and address
 *
 * Address and port is resolved with gethostbyname which was recommended 
 * to by on the ISA forum
 *
 * @param[out] int socket where the connection is setup
 * @param[out] string address set by user where the server should be
 * @param[out] int port set by user where the server should be
 * @return int which signalizes a return code (0 is a success)
 */
int createSocketAndConnect(int &sock, string &address, int &port);