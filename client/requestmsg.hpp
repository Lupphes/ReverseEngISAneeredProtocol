/**
 * ISA Projekt
 * Ondřej Sloup – (xsloup02)
 * Reverse-engineering neznámeho protokolu (Ing. Koutenský)
 * 
 * requestmsg.hpp
 * v1.0 – 23.10.2021
 */

#pragma once

#include <string.h>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

/* Virtual class for message parsing and handling */
class RequestMsg {
protected:
    string request; // Name of the command
    int numberOfArgs; // Number of parameters
    
public:
    RequestMsg(string request, int numberOfArgs);
    virtual ~RequestMsg();

    /**
     * Builds string from given arguments and returns it
     * Escape chars 
     *
     * @param[in] vector<string> number of recieved arguments
     * @param[out] string returns the built string for a server
     * @return int which signalizes a return code (0 is a success)
     */
    virtual int buildString(vector<string> commandArgs, string &result) = 0;

    /**
     * Parses the output from the server and prints result
     * Unescape chars 
     *
     * @param[out] string returns the builded string for server
     * @return int which signalizes a return code (0 is a success)
     */
    virtual int handleOutput(string &out) = 0;

    /**
     * Prints error when there are no or invalid parameters
     */
    virtual void getError() = 0;

    /**
     * Returns the number of the arguments
     * 
     * @return int returns the number of the arguments
     */
    int getNumArg();

    /**
     * Converts password to base64 format
     * I learnt about base64 from this page:
     * https://base64.guru/learn/base64-algorithm/encode
     * and implemented the algorithm how it was presented there,
     * but I added padding as it is requirement in this case
     * 
     * @param[in] string passwords which will be converted
     * @return string returns converted string to base64
     */
    string toBase64(string pass);

    /**
     * Get token from a file
     *
     * @param[out] string variable where the token is saved
     * @return int which signalizes a return code (0 is a success)
     */
    int getToken(string &token);

    /**
     * Create a file and insert token there
     *
     * @param[in] string token which is saved
     * @return int which signalizes a return code (0 is a success)
     */
    int createToken(string token);

    /**
     * Delete a token file
     *
     * @return int which signalizes a return code (0 is a success)
     */
    int removeToken();

    /**
     * Add "success" or "error" to the output message and prints it
     *
     * @param[out] string message which should be printed
     * @return int which signalizes a return code (0 is a success)
     */
    int resultParse(string &out);

    /**
     * Add "success" or "error" to the output message and prints it
     *
     * @param[out] string message which should be printed
     * @param[out] int if error or success
     * @return int which signalizes a return code (0 is a success)
     */
    void printResult(string &result, int code);

    /**
     * Escape characters in build string which will be 
     * send to server
     *
     * @param[out] vector<string> message string
     * @return int which signalizes a return code (0 is a success)
     */
    int escapeChars(vector<string> &input);

    /**
     * Unescape characters sent from the server
     *
     * @param[out] string message string
     * @return int which signalizes a return code (0 is a success)
     */
    int unescapeChars(string &input);

    /**
     * Checks if the passed number is a number
     *
     * @param[in] string supposed number
     * @return int which signalizes a return code (0 is a success)
     */
    int isNumber(string number);

    /**
     * Splits message data from the server by quotation marks
     * therefore they can be processed
     *
     * @param[in] string string to be splitted
     * @param[out] vector<string> values splitted
     * @return int which signalizes a return code (0 is a success)
     */
    int splitByRegex(string str, vector<string> &matches);
};

/* Class for parsing register commands */
class Register : public RequestMsg {
    public:
        Register();

        /**
         * Builds string from given arguments and returns it
         * Escape chars 
         *
         * @param[in] vector<string> number of recieved arguments
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int buildString(vector<string> commandArgs, string &result);
        int handleOutput(string &out);
        void getError();
};

/* Class for parsing login commands */
class Login : public RequestMsg {
    public:
        Login();

        /**
         * Builds string from given arguments and returns it
         * Escape chars 
         *
         * @param[in] vector<string> number of recieved arguments
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int buildString(vector<string> commandArgs, string &result);

        /**
         * Parses the output from the server and prints result
         * Unescape chars 
         *
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int handleOutput(string &out);

        /**
         * Prints error when there are no or invalid parameters
         */
        void getError();
};

/* Class for parsing list commands */
class List : public RequestMsg {
    public:
        List();

        /**
         * Builds string from given arguments and returns it
         * Escape chars 
         *
         * @param[in] vector<string> number of recieved arguments
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int buildString(vector<string> commandArgs, string &result);

        /**
         * Parses the output from the server and prints result
         * Unescape chars 
         *
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int handleOutput(string &out);

        /**
         * Prints error when there are no or invalid parameters
         */
        void getError();
};

/* Class for parsing fetch commands */
class Fetch : public RequestMsg {
    public:
        Fetch();

        /**
         * Builds string from given arguments and returns it
         * Escape chars 
         *
         * @param[in] vector<string> number of recieved arguments
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int buildString(vector<string> commandArgs, string &result);

        /**
         * Parses the output from the server and prints result
         * Unescape chars 
         *
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int handleOutput(string &out);

        /**
         * Prints error when there are no or invalid parameters
         */
        void getError();
};

/* Class for parsing send commands */
class Send : public RequestMsg {
    public:
        Send();

        /**
         * Builds string from given arguments and returns it
         * Escape chars 
         *
         * @param[in] vector<string> number of recieved arguments
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int buildString(vector<string> commandArgs, string &result);

        /**
         * Parses the output from the server and prints result
         * Unescape chars 
         *
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int handleOutput(string &out);

        /**
         * Prints error when there are no or invalid parameters
         */
        void getError();
};

/* Class for parsing logout commands */
class Logout : public RequestMsg {
    public:
        Logout();

        /**
         * Builds string from given arguments and returns it
         * Escape chars 
         *
         * @param[in] vector<string> number of recieved arguments
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int buildString(vector<string> commandArgs, string &result);

        /**
         * Parses the output from the server and prints result
         * Unescape chars 
         *
         * @param[out] string returns the builded string for server
         * @return int which signalizes a return code (0 is a success)
         */
        int handleOutput(string &out);

        /**
         * Prints error when there are no or invalid parameters
         */
        void getError();
};