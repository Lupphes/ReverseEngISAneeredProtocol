
#pragma once

#include <string.h>
#include <vector>
#include <map>
#include <iostream>


using namespace std;

class RequestMsg {
protected:
    // Need to be protected so it is visible to children
    string request;
    int numberOfArgs;
    
public:
    // Initializer list to initialize request
    RequestMsg(string request, int numberOfArgs);
    virtual string buildString(vector<string> commnadArgs) = 0;
    int getNumArg();
    string toBase64(string pass);
    int getToken(string* token);
    int createToken(string token);
    int removeToken();
    virtual int handleOutput(string &out) = 0;
    int resultParse(string* out);
    void printResult(string* result, int code);
    virtual void getError() = 0;
};

class Register : public RequestMsg {
    // Call super constructor with message
    public:
        Register();
        string buildString(vector<string> commnadArgs);
        int handleOutput(string &out);
        void getError();
};

class Login : public RequestMsg {
    // Call super constructor with message
    public:
        Login();
        string buildString(vector<string> commnadArgs);
        int handleOutput(string &out);
        void getError();
};

class List : public RequestMsg {
    // Call super constructor with message
    public:
        List();
        string buildString(vector<string> commnadArgs);
        int handleOutput(string &out);
        void getError();
};

class Fetch : public RequestMsg {
    // Call super constructor with message
    public:
        Fetch();

        string buildString(vector<string> commnadArgs);
        int handleOutput(string &out);
        void getError();
};

class Send : public RequestMsg {
    // Call super constructor with message
    public:
        Send();

        string buildString(vector<string> commnadArgs);
        int handleOutput(string &out);
        void getError();
};

class Logout : public RequestMsg {
    // Call super constructor with message
    public:
        Logout();

        string buildString(vector<string> commnadArgs);
        int handleOutput(string &out);
        void getError();
};