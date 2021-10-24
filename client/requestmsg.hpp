
#pragma once

#include <string.h>
#include <vector>
#include <map>
#include <iostream>


using namespace std;

class RequestMsg {
protected:
    string request;
    int numberOfArgs;
    
public:
    RequestMsg(string request, int numberOfArgs);
    virtual ~RequestMsg();
    virtual int buildString(vector<string> commnadArgs, string &result) = 0;
    virtual int handleOutput(string &out) = 0;
    virtual void getError() = 0;

    int getNumArg();

    string toBase64(string pass);

    int getToken(string &token);
    int createToken(string token);
    int removeToken();

    int resultParse(string &out);
    void printResult(string &result, int code);

    int escapeChars(vector<string> &input);
    int unescapeChars(string &input);
    int isNumber(string number);
    int splitByRegex(string str, vector<string> &matches);
};

class Register : public RequestMsg {
    // Call super constructor with message
    public:
        Register();
        int buildString(vector<string> commnadArgs, string &result);
        int handleOutput(string &out);
        void getError();
};

class Login : public RequestMsg {
    // Call super constructor with message
    public:
        Login();
        int buildString(vector<string> commnadArgs, string &result);
        int handleOutput(string &out);
        void getError();
};

class List : public RequestMsg {
    // Call super constructor with message
    public:
        List();
        int buildString(vector<string> commnadArgs, string &result);
        int handleOutput(string &out);
        void getError();
};

class Fetch : public RequestMsg {
    // Call super constructor with message
    public:
        Fetch();

        int buildString(vector<string> commnadArgs, string &result);
        int handleOutput(string &out);
        void getError();
};

class Send : public RequestMsg {
    // Call super constructor with message
    public:
        Send();

        int buildString(vector<string> commnadArgs, string &result);
        int handleOutput(string &out);
        void getError();
};

class Logout : public RequestMsg {
    // Call super constructor with message
    public:
        Logout();

        int buildString(vector<string> commnadArgs, string &result);
        int handleOutput(string &out);
        void getError();
};