// client.h : Include file for standard system include files,
// or project specific include files.

#pragma once

enum returnCodes {
    SUCCESS = 0,
    ERR_NO_ARGS = 3,
    ERR_ARG,
    ERR_UNKNOWN_COMMAND,
    ERR_BUILDING_STRING,
    ERR_HOST_NOT_RESOLVED,
    ERR_SOCKET_ERROR,
    ERR_SERVER_CONNECTION,
    ERR_SEND_UNSUCCESSFUL,
    ERR_RECV_UNSUCCESSFUL,
    ERR_INCORRECT_OUTPUT
};