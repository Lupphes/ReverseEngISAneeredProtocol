/**
 * ISA Projekt
 * Ondřej Sloup – (xsloup02)
 * Reverse-engineering neznámeho protokolu (Ing. Koutenský)
 * 
 * handler.hpp
 * v1.0 – 23.10.2021
 */

#pragma once

#include <map>
#include "requestmsg.hpp"
#include "errorcodes.hpp"
#include <vector>
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex> // For exceptions
#define bufferlength 60 // Length of the array set to an average length of packet

using namespace std;

/* Class which determines the correct class and link them to the command */
class RequestHandler {
private:
    /* Map which links class to command */
    map<string, RequestMsg*> request_dict = {
        {"register", new Register()},
        {"login", new Login()},
        {"list", new List()},
        {"send", new Send()},
        {"fetch", new Fetch()},
        {"logout", new Logout()}
    };
    RequestMsg* command;

public:
    RequestHandler() {}

    /**
     * Destroys all the classes in map
     * 
     */
    ~RequestHandler() {
        map<string, RequestMsg*>::iterator it;
        for (it = this->request_dict.begin(); it != this->request_dict.end(); it++) {
            delete it->second;
        }
    }

    /**
     * Function which identifies which command is input and
     * validates all the inputs
     * 
     *  Creates a class that determines what needs to be done
     *
     * @param[in] int number of arguments
     * @param[in] char** list of the arguments
     * @param[out] int vector of command variables
     * @param[out] string string prepared to send to the server
     * @return int which signalizes a return code (0 is a success)
     */
    int buildClientString(int argc, char** argv, int &args_processed, string &builtString);

    /**
     * Exchanging data between client and server and handling
     * the output.
     * 
     * Socket is created in another function
     * 
     * @param[out] int opened socket where data will be sent
     * @param[out] string string prepared to send to the server
     * @return int which signalizes a return code (0 is a success)
     */
    int exchangeData(int &sock, string &builtString);
};