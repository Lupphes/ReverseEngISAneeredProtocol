/**
 * ISA Projekt
 * Ondřej Sloup – (xsloup02)
 * Reverse-engineering neznámeho protokolu (Ing. Koutenský)
 * 
 * errorcodes.cpp
 * v1.0 – 23.10.2021
 */

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
    ERR_HOST_NOT_RESOLVED, // Hostname couldn't be resolved
    ERR_GET_ADDR_INFO, // Cannot get address info
    ERR_SERVER_CONNECTION, // Server connection error
    ERR_SEND_UNSUCCESSFUL, // Cannot send a message
    ERR_RECV_UNSUCCESSFUL, // Cannot receive a message
    ERR_INCORRECT_OUTPUT, // Incorrect command error
    ERR_PACKET_INVALID_FORMAT, // Packet is in invalid format
    ERR_CANT_OPEN_FILE, // Cannot open a token file
    ERR_CANT_CREATE_FILE, // Cannot create a token file
    ERR_CANT_DELETE_FILE, // Cannot delete a token file
    SERVER_ERR, // General server error
    ERR_UNEXPECTED_ERROR, // Error inside std library
    ERR_UNEXPECTED_REGEX_ERROR, // Error inside regex library
    ERR_UNEXPECTED_PARSING_ERROR // Error inside vector/string library
};
