
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
    string getToken();
    int createToken(string token);
    virtual int handleOutput(string out) = 0;
    int resultParse(string* out);
    void printResult(string* result, int code);

};

class Register : public RequestMsg {
    // Call super constructor with message
    public:
        Register();

    private:
        string buildString(vector<string> commnadArgs);
        int handleOutput(string out);
};

class Login : public RequestMsg {
    // Call super constructor with message
    public:
        Login();

    private:
        string buildString(vector<string> commnadArgs);
        int handleOutput(string out);
};

class List : public RequestMsg {
    // Call super constructor with message
    public:
        List();

    private:
        string buildString(vector<string> commnadArgs);
        int handleOutput(string out);
};

class Fetch : public RequestMsg {
    // Call super constructor with message
    public:
        Fetch();

    private:
        string buildString(vector<string> commnadArgs);
        int handleOutput(string out);
};

class Send : public RequestMsg {
    // Call super constructor with message
    public:
        Send();

    private:
        string buildString(vector<string> commnadArgs);
        int handleOutput(string out);
};

class Logout : public RequestMsg {
    // Call super constructor with message
    public:
        Logout();

    private:
        string buildString(vector<string> commnadArgs);
        int handleOutput(string out);
};