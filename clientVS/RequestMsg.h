
#pragma once

#include <string.h>
#include <map>

using namespace std;

class RequestMsg {
protected:
    // Need to be protected so it is visible to children
    string request;
    int numberOfArgs;
    
public:
    // Initializer list to initialize request
    RequestMsg(string request, int numberOfArgs);
    virtual int validate() = 0;
};

class Register : public RequestMsg {
    // Call super constructor with message
    public:
        Register();

    private:
        int validate();
};

class Login : public RequestMsg {
    // Call super constructor with message
    public:
        Login();

    private:
        int validate();
};

class List : public RequestMsg {
    // Call super constructor with message
    public:
        List();

    private:
        int validate();
};

class Fetch : public RequestMsg {
    // Call super constructor with message
    public:
        Fetch();

    private:
        int validate();
};

class Send : public RequestMsg {
    // Call super constructor with message
    public:
        Send();

    private:
        int validate();
};

class Logout : public RequestMsg {
    // Call super constructor with message
    public:
        Logout();

    private:
        int validate();
};

map<string, RequestMsg*> request_dict {
        {"register", new Register()},
        {"login", new Login()},
        {"list", new List()},
        {"send", new Send()},
        {"fetch", new Fetch()},
        {"logout", new Logout()}
};