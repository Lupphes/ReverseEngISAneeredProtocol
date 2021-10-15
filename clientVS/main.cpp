#include "handler.hpp"
#include "client.h"

int main(int argc, char **argv) {

    string address = "localhost";
    int port = 32323;
    int args_processed;
    string builtString;
    int retCode;

    RequestHandler genMap;

    int sock;

    cout << "parsing args" << endl;
    
    if ((retCode = parseArgs(argc, argv, args_processed, address, port)) != 0) {
        return retCode;
    }

    cout << "build client" << endl;
    if ((retCode = genMap.buildClientString(argc, argv, args_processed, builtString)) != 0)  {
        return retCode;
    }

    cout << "create socket" << endl;

    if ((retCode = createSocketAndConnect(sock, address, port)) != 0) {
        close(sock);
        return retCode;
    }
    cout << "senddata" << endl;
    if ((retCode = genMap.sendDataToServer(sock, builtString)) != 0) {
        close(sock);
        return retCode;
    }

    close(sock);
    return returnCodes::SUCCESS;
}