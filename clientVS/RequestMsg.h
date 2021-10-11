
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

};

class Register : public RequestMsg {
    // Call super constructor with message
    public:
        Register();

    private:
        string buildString(vector<string> commnadArgs);
};

class Login : public RequestMsg {
    // Call super constructor with message
    public:
        Login();

    private:
        string buildString(vector<string> commnadArgs);
};

class List : public RequestMsg {
    // Call super constructor with message
    public:
        List();

    private:
        string buildString(vector<string> commnadArgs);
};

class Fetch : public RequestMsg {
    // Call super constructor with message
    public:
        Fetch();

    private:
        string buildString(vector<string> commnadArgs);
};

class Send : public RequestMsg {
    // Call super constructor with message
    public:
        Send();

    private:
        string buildString(vector<string> commnadArgs);
};

class Logout : public RequestMsg {
    // Call super constructor with message
    public:
        Logout();

    private:
        string buildString(vector<string> commnadArgs);
};