#pragma once

/* This is enum class of all return codes */
enum returnCodes {
    SUCCESS = 0, // Success
    PRINT_HELP = 3, // User was presented with help
    ERR_NO_ARGS, // No arguments were set to client
    ERR_ARG, // Wrong arguments were set to client
    ERR_UNKNOWN_COMMAND, // Unknown command was set by user
    ERR_ID_NOT_NUMBER, // Command fetch got parameter which was not a number
    ERR_BUILDING_STRING, // Error while building a string which will be send to server
    ERR_HOST_NOT_RESOLVED,
    ERR_SOCKET_ERROR,
    ERR_CANT_RESOLVE,
    ERR_GET_ADDR_INFO,
    ERR_SERVER_CONNECTION,
    ERR_SEND_UNSUCCESSFUL,
    ERR_RECV_UNSUCCESSFUL,
    ERR_INCORRECT_OUTPUT,
    ERR_PACKET_INVALID_FORMAT,
    ERR_CANT_OPEN_FILE,
    ERR_CANT_CREATE_FILE,
    ERR_CANT_DELETE_FILE,
    SERVER_ERR,
    ERR_UNEXPECTED_ERROR,
    ERR_UNEXPECTED_REGEX_ERROR,
    ERR_UNEXPECTED_PARSING_ERROR
};
