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

void printHelpMessage() {
    std::cout <<
            "usage: client [ <option> ... ] <command> [<args>] ...\n\n"
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
            "\tlogin <username> <password>\n";
            "\tlist\n";
            "\tsend <recipient> <subject> <body>\n";
            "\tfetch <id>\n";
            "\tlogout\n";
    exit(1);
}

string parseArguments(int argc, char** argv) {
    int args_processed = 1;

    if (argc == 1) {
        printf("client: expects <command> [<args>] ... on the command line, given 0 arguments\n");
        exit(1);
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
        printf("unknown command\n");
        exit(1);
    }

    printf("Address is set to: %s \nPort is set to: %d\n", address.c_str(), port);

    command = request_dict.at(argv[args_processed]);
    if (args_processed + command->getNumArg() != argc - 1) {
        //command->getError()
        printf("Incorrect number of arugments\n");
        exit(1);
    }
    
    vector<string> commnadArgs;
    for (int i = args_processed + 1; i < argc; i++) {
        commnadArgs.push_back(argv[i]);
    }

    string result;
    if ((result = command->buildString(commnadArgs)) == "") {
        exit(1);
    }
    return result;

}

int parseMsg() {
    return 0;
}

void sendRequest(int sockfd, string result) {
    int sendInfo, readInfo;
    char const* createMessage = result.c_str();
    char buffer[1024] = {0};

    sendInfo = send(sockfd, createMessage, strlen(createMessage), 0);
    readInfo = read(sockfd, buffer, 1024);

    // printf("%s\n", buffer);

    command->handleOutput(buffer);
}

int main(int argc, char **argv) {
    string result = parseArguments(argc, argv);


    // Create socket for TCP communication
    int socks = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr, cli;
   
    if (socks == -1) {
        printf("Socket couldn't be created!\n");
        exit(1);
    }
    else
        printf("Socket was created!\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(address.c_str());
    servaddr.sin_port = htons(port);
   
    // connect the client socket to server socket
    if (connect(socks, (sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection to server failed!\n");
        exit(0);
    }
    else
        printf("Connected to server!\n");
   
    // function for chat
    sendRequest(socks, result);
   
    // close the socket
    close(socks);

    return 0;
}