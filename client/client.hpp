// client.h : Include file for standard system include files,
// or project specific include files.

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

void printHelpMessage();
int parseArgs(int argc, char** argv, int &args_processed, string& address, int &port);
int createSocketAndConnect(int &sock, string& address, int &port);