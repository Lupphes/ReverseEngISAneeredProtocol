/**
 * ISA Projekt
 * Ondřej Sloup – (xsloup02)
 * Reverse-engineering neznámeho protokolu (Ing. Koutenský)
 * 
 * client.cpp
 * v1.0 – 23.10.2021
 */

#include "client.hpp"

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

int parseArgs(int argc, char** argv, int &args_processed, string &address, int &port) {
    args_processed = 1;
    // I used regex from this website to make sure that the port is correct
    // https://ihateregex.io/expr/port/
    // https://en.cppreference.com/w/cpp/string/basic_string/stol
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

        if (opt == -1)
            break;

        switch (opt) {
            case 'a':
                address = optarg;
                args_processed += 2;
                break;

            case 'p':
                if (regex_match(optarg, regPort)) {
                    try {
                        port = stoi(optarg, nullptr);
                    } catch (const invalid_argument & e) {
                        cerr << "Port number is not valid\n";
                        return returnCodes::ERR_ARG;
                    } catch (const out_of_range & e) {
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

int createSocketAndConnect(int &sock, string& address, int &port) {  
    // Prepraring values for connection and socket
    int retCode;
    struct addrinfo ipAddress = {}, *res;
    ipAddress.ai_family = AF_UNSPEC;
    ipAddress.ai_socktype = SOCK_STREAM;
    ipAddress.ai_protocol = IPPROTO_TCP;

    // Resolving a hostname
    // I used snippets from this website: 
    // https://stackoverflow.com/questions/52727565/client-in-c-use-gethostbyname-or-getaddrinfo
    // I decided not to use gethostbyname() nor gethostbyname2() since IPv6 is not supported
    // and it is not the best approach  
    if ((retCode = getaddrinfo(address.c_str(), to_string(port).c_str(), &ipAddress, &res)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(retCode) << "\n";
        return returnCodes::ERR_GET_ADDR_INFO;
    }

    // Resolve hostname
    for(struct addrinfo *addr = res; addr != NULL; addr = addr->ai_next) {
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock == -1) {
            retCode = errno;
            continue;
        }

        // Connect to server
        if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
            break;

        retCode = errno;
        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);

    if (sock == -1) {
        cerr << strerror(retCode) << ": " << address << "\n";
        return returnCodes::ERR_CANT_RESOLVE;
    }

    return returnCodes::SUCCESS;
}
