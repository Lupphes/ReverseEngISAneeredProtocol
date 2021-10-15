#include "handler.hpp"
#include "client.hpp"


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