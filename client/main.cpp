/**
 * ISA Projekt
 * Ondřej Sloup – (xsloup02)
 * Reverse-engineering neznámeho protokolu (Ing. Koutenský)
 * 
 * main.cpp
 * v1.0 – 23.10.2021
 */

#include "handler.hpp"
#include "client.hpp"

/**
 * Main function of TCP client which replicates the ISA client
 *
 * This is the main function which launches the whole process.
 * It is divided into four steps: 
 *      1) Parsing the arguments
 *      2) Building string from the arguments which will be sent to the client
 *      3) Connecting to the socket
 *      4) Exchange of the data between client and server
 *
 * @param[in] argc number of arguments
 * @param[in] argv list of the arguments
 * @return int which signalizes a return code (0 is a success)
 */
int main(int argc, char **argv) {

    string address = "localhost";
    int port = 32323;
    
    int args_processed, sock, retCode;
    string builtString;
    RequestHandler genMap;

    // DEBUG: Status print
    // cout << "Parse the arguments:" << endl;
    
    if ((retCode = parseArgs(argc, argv, args_processed, address, port)) != 0) {
        return retCode;
    }

    // DEBUG: Status print
    // cout << "Build client string" << endl;

    // DEBUG: String for printing out the address and port
    // cout << address << ":" << to_string(port) << endl;

    if ((retCode = genMap.buildClientString(argc, argv, args_processed, builtString)) != 0)  {
        return retCode;
    }

    // DEBUG: Status print
    // cout << "Create an socket and connect to server" << endl;

    if ((retCode = createSocketAndConnect(sock, address, port)) != 0) {
        close(sock);
        return retCode;
    }

    // DEBUG: Status print
    // cout << "Send and Recieve data" << endl;

    if ((retCode = genMap.exchangeData(sock, builtString)) != 0) {
        close(sock);
        return retCode;
    }

    close(sock);
    return returnCodes::SUCCESS;
}