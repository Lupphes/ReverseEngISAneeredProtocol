// client.cpp : Defines the entry point for the application.
//

#include "RequestMsg.h"

#include <getopt.h>
#include <vector>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

string address = "localhost";
int port = 32323;
RequestMsg* command;

map<string, RequestMsg*> request_dict = {
        {"register", new Register()},
        {"login", new Login()},
        {"list", new List()},
        {"send", new Send()},
        {"fetch", new Fetch()},
        {"logout", new Logout()}
};

enum returnCodes {
    SUCCESS = 0,
    ERR_NO_ARGS,
    ERR_ARG,
    ERR_UNKNOWN_COMMAND,
    ERR_BUILDING_STRING,
    ERR_SOCKET_ERROR,
    ERR_SERVER_CONNECTION
};


void printHelpMessage() {
    cout << "usage: client [ <option> ... ] <command> [<args>] ...\n\n"
            "<option> is one of\n\n"
            "-a <addr>, --address <addr>\n"
            "\tServer hostname or address to connect to\n"
            "-p <port>, --port <port>\n"
            "\tServer port to connect to\n"
            "--help, -h\n"
            "\tShow this help\n"
            "--\n"
            "\tDo not treat any remaining argument as a switch (at this level)\n\n"
            "Multiple single-letter switches can be combined after\n"
            "Supported commands:\n"
            "\tregister <username> <password>\n"
            "\tlogin <username> <password>\n"
            "\tlist\n"
            "\tsend <recipient> <subject> <body>\n"
            "\tfetch <id>\n"
            "\tlogout\n";
    exit(returnCodes::SUCCESS);
}

string parseArgsBuildString(int argc, char** argv) {
    int args_processed = 1;

    if (argc == 1) {
        printf("client: expects <command> [<args>] ... on the command line, given 0 arguments\n");
        exit(ERR_NO_ARGS);
    }

    const char* const short_opts = "a:p:h";
    const option long_opts[] = {
            {"address", required_argument, nullptr, 'a'},
            {"port", required_argument, nullptr, 'p'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt) {
            case 'a':
                address = optarg;
                args_processed += 2;
                break;

            case 'p':
                port = std::stoi(optarg);
                args_processed += 2;
                break;

            case 'h': // -h or --help
            case '?': // Unrecognized option
            default:
                printHelpMessage();
                break;
        }
    }

    if (argv[args_processed] == 0 || request_dict.find(argv[args_processed]) == request_dict.end()) {
        cout << "unknown command\n";
        exit(returnCodes::ERR_ARG);
    }

    command = request_dict.at(argv[args_processed]);

    // Debuging string for printing out the address and port
    // cout << "Address is set to: " + address + "\nPort is set to: " + to_string(port) + "\n";

    if (args_processed + command->getNumArg() != argc - 1) {
        command->getError();
        exit(returnCodes::ERR_UNKNOWN_COMMAND);
    }
    
    vector<string> commnadArgs;
    for (int i = args_processed + 1; i < argc; i++) {
        commnadArgs.push_back(argv[i]);
    }

    string result;
    if ((result = command->buildString(commnadArgs)) == "") {
        exit(returnCodes::ERR_BUILDING_STRING);
    }
    return result;
}

void sendDataToServer(int sock, string result) {
    int sendCode, readCode;
    char const* builtString = result.c_str();
    char buffer[1024] = {0};

    sendCode = send(sock, builtString, strlen(builtString), 0);
    readCode = read(sock, buffer, 1024);

    command->handleOutput(buffer);
}

int main(int argc, char **argv) {
    string builtString = parseArgsBuildString(argc, argv);

    // TCP Client code
    // Creating a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in ipAddress;
   
    // Creating of the socket for comunication
    if (sock == -1) {
        cout << "Can't create a socket\n";
        exit(returnCodes::ERR_SOCKET_ERROR);
    }
   
    // Prepare connection
    ipAddress.sin_family = AF_INET;
    ipAddress.sin_addr.s_addr = inet_addr(address.c_str());
    ipAddress.sin_port = htons(port);
   
    // Connection to the socket 
    if (connect(sock, (sockaddr*)&ipAddress, sizeof(ipAddress)) < 0) {
        cout << "Client can't connect to the server\n";
        exit(returnCodes::ERR_SERVER_CONNECTION);
    }
   
    sendDataToServer(sock, builtString);

    close(sock);

    return returnCodes::SUCCESS;
}