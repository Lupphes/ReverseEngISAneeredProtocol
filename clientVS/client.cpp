// client.cpp : Defines the entry point for the application.
//

#include "RequestMsg.h"
#include "client.h"

#include <getopt.h>
#include <vector>
#include <iostream>
#include <regex>
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
#define bufferLenght 60 // Length of the array set to an average lenght of packet

class RequestHandler {
public:
    map<string, RequestMsg*> request_dict = {
        {"register", new Register()},
        {"login", new Login()},
        {"list", new List()},
        {"send", new Send()},
        {"fetch", new Fetch()},
        {"logout", new Logout()}
    };


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
}

int parseArgs(int argc, char** argv, int &args_processed) {
    args_processed = 1;
    regex regPort("^((6553[0-5])|(655[0-2][0-9])|(65[0-4][0-9]{2})|(6[0-4][0-9]{3})|([1-5][0-9]{4})|([0-5]{0,5})|([0-9]{1,4}))$");

    if (argc == 1) {
        cerr << "client: expects <command> [<args>] ... on the command line, given 0 arguments\n";
        return returnCodes::ERR_NO_ARGS;
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
                // https://ihateregex.io/expr/port/
                // https://en.cppreference.com/w/cpp/string/basic_string/stol
                if (regex_match(optarg, regPort)) {
                    try {
                        port = std::stoi(optarg, nullptr);
                    } catch (const std::invalid_argument & e) {
                        cerr << "Port number is not valid\n";
                        return returnCodes::ERR_ARG;
                    } catch (const std::out_of_range & e) {
                        cerr << "Port number is out of range\n";
                        return returnCodes::ERR_ARG;
                    }
                } else {
                    cerr << "Port number is not a string or not valid\n";
                    return returnCodes::ERR_ARG;
                }
                args_processed += 2;    
                break;

            case 'h': // -h or --help
            case '?': // Unrecognized option
            default:
                printHelpMessage();
                return returnCodes::PRINT_HELP;
                break;
        }
    }


    return returnCodes::SUCCESS;
}

int buildClientString(int argc, char** argv, int &args_processed, string &builtString, RequestMsg *&command, RequestHandler &handler) {

    if (argv[args_processed] == 0 || handler.request_dict.find(argv[args_processed]) == handler.request_dict.end()) {
        cerr << "unknown command\n";
        return returnCodes::ERR_ARG;
    }

    command = handler.request_dict.at(argv[args_processed]);


    // DEBUG: String for printing out the address and port
    // cout << address << ":" << to_string(port) << endl;

    if ((args_processed + command->getNumArg()) != (argc - 1)) {
        command->getError();
        return returnCodes::ERR_UNKNOWN_COMMAND;
    }
    
    vector<string> commnadArgs;
    for (int i = args_processed + 1; i < argc; i++) {
        commnadArgs.push_back(argv[i]);
    }

    builtString = command->buildString(commnadArgs);
    if (builtString == "") {
        return returnCodes::ERR_BUILDING_STRING;
    }

    return returnCodes::SUCCESS;
}

int sendDataToServer(int &sock, string &builtString, RequestMsg *&command) {
    int sendCode, readCode;
    char buffer[bufferLenght] = {0}; 
    string recievedString;
    
    sendCode = send(sock, builtString.c_str(), builtString.length(), 0);
    if (sendCode != -1) {
        while (readCode = recv(sock, buffer, bufferLenght, 0)) {
            if (readCode == -1) {
                cerr << "Response cannot be read\n";
                return returnCodes::ERR_RECV_UNSUCCESSFUL;
            }
                recievedString += buffer;
        }

    } else {
        cerr << "Request cannot not be send\n";
        return returnCodes::ERR_SEND_UNSUCCESSFUL;
    }
    
    if (command->handleOutput(recievedString) != returnCodes::SUCCESS) {
        return returnCodes::ERR_INCORRECT_OUTPUT;
    }

    return returnCodes::SUCCESS;
}

int createSocketAndConnect(int &sock) {
    // TCP Client code
    
    // Resolving hostname
    struct sockaddr_in ipAddress;
    struct hostent* hostname;

    // Prepare connection
    hostname = gethostbyname(address.c_str());
    if (hostname == NULL) {
        cerr << "Couldn't find a host\n";
        return returnCodes::ERR_HOST_NOT_RESOLVED;
    }

    // Creating a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Creating of the socket for comunication
    if (sock == -1) {
        cout << "Can't create a socket\n";
        return returnCodes::ERR_SOCKET_ERROR;
    }
   
    ipAddress.sin_family = AF_INET; 
    ipAddress.sin_addr.s_addr = inet_addr(inet_ntoa (*((struct in_addr*)hostname->h_addr_list[0])));
    ipAddress.sin_port = htons(port);

    // DEBUG: String for outputing IP and port
    // cout << inet_ntoa (*((struct in_addr*)hostname->h_addr_list[0])) << ":" << port << endl;
   
    // Connection to the socket 
    if (connect(sock, (sockaddr*)&ipAddress, sizeof(ipAddress)) < 0) {
        cerr << "Client can't connect to the server\n";
        return returnCodes::ERR_SERVER_CONNECTION;
    }
   
    return returnCodes::SUCCESS;
}



int main(int argc, char **argv) {
    RequestHandler genMap;
    int args_processed;
    string builtString;
    int retCode;

    RequestMsg* command;
    int sock;

    cout << "parsing args" << endl;
    
    if ((retCode = parseArgs(argc, argv, args_processed)) != 0) {
        return retCode;
    }

    cout << "build client" << endl;
    if ((retCode = buildClientString(argc, argv, args_processed, builtString, command, genMap)) != 0)  {
        return retCode;
    }

    cout << "create socket" << endl;

    if ((retCode = createSocketAndConnect(sock)) != 0) {
        close(sock);
        return retCode;
    }
    cout << "senddata" << endl;
    if ((retCode = sendDataToServer(sock, builtString, command)) != 0) {
        close(sock);
        return retCode;
    }

    close(sock);
    return returnCodes::SUCCESS;
}