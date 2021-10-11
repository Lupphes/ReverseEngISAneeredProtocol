// client.cpp : Defines the entry point for the application.
//

#include "client.h"

#include <getopt.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

char* address;
int port;

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

void parseArguments(int argc, char** argv) {
    int args_processed = 1;

    const char* const short_opts = "a:p:h";
    const option long_opts[] = {
            {"address", required_argument, nullptr, 'a'},
            {"port", required_argument, nullptr, 'p'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
        {
        case 'a':
            address = optarg;
            args_processed += 2;
            std::cout << "Address is set to: " << address << std::endl;
            break;

        case 'p':
            port = std::stoi(optarg);
            args_processed += 2;
            std::cout << "Port is set to: " << port << std::endl;
            break;

        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            printHelpMessage();
            break;
        }
    }

    if (request_dict.find(argv[args_processed]) == request_dict.end()) {
        printf("Request %s doesn't exist", argv[args_processed]);
        exit(1);
    }

    RequestMsg* command = request_dict.at(argv[args_processed]);


    command->validate();

}

void sendRequest(int sockfd) {
    int sendInfo, readInfo;
    char createMessage[] = "(login  \"test\" \"test\")";
    char buffer[1024] = {0};


    sendInfo = send(sockfd, createMessage, strlen(createMessage), 0 );
    readInfo = read(sockfd, buffer, 1024);


    printf("%s\n", buffer);
}

int main(int argc, char **argv) {
    parseArguments(argc, argv);


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
    servaddr.sin_addr.s_addr = inet_addr(address);
    servaddr.sin_port = htons(port);
   
    // connect the client socket to server socket
    if (connect(socks, (sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection to server failed!\n");
        exit(0);
    }
    else
        printf("Connected to server!\n");
   
    // function for chat
    sendRequest(socks);
   
    // close the socket
    close(socks);

    return 0;
}