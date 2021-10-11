#include "RequestMsg.h"

#include <bitset>
#include <vector>


string RequestMsg::toBase64(string pass) {
    string base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    vector<char> char_array(pass.begin(), pass.end());

    string binaryStr;

    string padding;
    for (const char &c: char_array)
        binaryStr += bitset<8>(c).to_string();
    if((binaryStr.length() % 3) == 1) {
        padding += "==";
    } else if ((binaryStr.length() % 3) == 2) {
        padding += "==";
    }


    while (binaryStr.length() % 6 != 0) {
        binaryStr += "0";
        
    }
    string final;
    int lenght_bin = binaryStr.length();
    for (int i = 0; i < lenght_bin; i+=6) {
        final += base64[stoi("00" + binaryStr.substr(i, 6), nullptr, 2)];
    }

    final += padding;

    printf("%s\n", final.c_str());
    return pass;
}


RequestMsg::RequestMsg(string request, int numberOfArgs): request(request), numberOfArgs(numberOfArgs) {
}

string RequestMsg::buildString(vector<string> commnadArgs) {
    return "";
}

int RequestMsg::getNumArg() {
    return RequestMsg::numberOfArgs;
}

Register::Register(): RequestMsg("register", 2) {
    
}

string Register::buildString(vector<string> commnadArgs) {
    string convPass = RequestMsg::toBase64(commnadArgs[1]);
    string builtStr = "(" + request + " \"" + commnadArgs[0] + "\" \"" + commnadArgs[1] + "\")";
    printf("%s\n", builtStr.c_str());
    return builtStr;
}

Login::Login(): RequestMsg("login", 2) {
    
}

string Login::buildString(vector<string> commnadArgs) {  
    string builtStr = "(" + request + " \"" + commnadArgs[0] + "\" \"" + commnadArgs[1] + "\")";
    printf("%s\n", builtStr.c_str());
    return builtStr;
}

List::List(): RequestMsg("list", 0) {
    
}

string List::buildString(vector<string> commnadArgs) {
    return "";
}

Fetch::Fetch(): RequestMsg("fetch", 1) {
    
}

string Fetch::buildString(vector<string> commnadArgs) {
    return "";
}

Send::Send(): RequestMsg("send", 3) {
    
}

string Send::buildString(vector<string> commnadArgs) {
    return "";
}

Logout::Logout(): RequestMsg("logout", 0){
    
}

string Logout::buildString(vector<string> commnadArgs) {
    return "";
}
