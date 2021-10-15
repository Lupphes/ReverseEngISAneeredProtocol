#pragma once

#include <map>
#include "RequestMsg.h"
#include "errorcodes.hpp"
#include <vector>
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define bufferLenght 60 // Length of the array set to an average lenght of packet

using namespace std;


class RequestHandler {
private:
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


    RequestHandler() {
        // Either leave empty or make it private and forbid instantiation using singleton
    }
    ~RequestHandler() {
        cout << "deconstructor called" << endl;
        map<string, RequestMsg*>::iterator it;
        for (it = this->request_dict.begin(); it != this->request_dict.end(); it++) {
            delete it->second;
        }
    }

    int buildClientString(int argc, char** argv, int &args_processed, string &builtString);
    int sendDataToServer(int &sock, string &builtString);
};