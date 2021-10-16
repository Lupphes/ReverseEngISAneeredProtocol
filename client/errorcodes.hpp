#pragma once

enum returnCodes {
    SUCCESS = 0,
    PRINT_HELP = 3,
    ERR_NO_ARGS,
    ERR_ARG,
    ERR_UNKNOWN_COMMAND,
    ERR_ID_NOT_NUMBER,
    ERR_BUILDING_STRING,
    ERR_HOST_NOT_RESOLVED,
    ERR_SOCKET_ERROR,
    ERR_SERVER_CONNECTION,
    ERR_SEND_UNSUCCESSFUL,
    ERR_RECV_UNSUCCESSFUL,
    ERR_INCORRECT_OUTPUT,
    ERR_PACKET_INVALID_FORMAT,
    ERR_CANT_OPEN_FILE,
    ERR_CANT_CREATE_FILE,
    ERR_CANT_DELETE_FILE,
    SERVER_ERR
};