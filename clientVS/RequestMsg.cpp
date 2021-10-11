#include "RequestMsg.h"
#include <string.h>

RequestMsg::RequestMsg(string request, int numberOfArgs): request(request), numberOfArgs(numberOfArgs) {

}
int RequestMsg::validate() {
    return 0;
}

Register::Register(): RequestMsg("register", 2) {
    
}
int Register::validate() {
    return 0;
}

Login::Login(): RequestMsg("login", 2) {
    
}
int Login::validate() {
    return 0;
}

List::List(): RequestMsg("list", 0) {
    
}
int List::validate() {
    printf("Fuck you");
    return 0;
}

Fetch::Fetch(): RequestMsg("fetch", 1) {
    
}
int Fetch::validate() {
    return 0;
}

Send::Send(): RequestMsg("send", 3) {
    
}
int Send::validate() {
    return 0;
}

Logout::Logout(): RequestMsg("logout", 0){
    
}
int Logout::validate() {
    return 0;
}

map<string, RequestMsg*> request_dict = {
        {"register", new Register()},
        {"login", new Login()},
        {"list", new List()},
        {"send", new Send()},
        {"fetch", new Fetch()},
        {"logout", new Logout()}
};