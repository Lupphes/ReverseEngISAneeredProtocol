#include "RequestMsg.h"

#include <bitset>
#include <vector>
#include <fstream>

void RequestMsg::printResult(string* result, int code) {
    if (code == 0) {
        printf("SUCCESS: %s\n", (*result).c_str());
    } else {
        printf("ERROR: %s\n", (*result).c_str());
    }
}

int RequestMsg::resultParse(string* out) {
    if ((*out).length() < 4) {
        exit(1);
    }
    (*out).erase(0,1).pop_back();
    printf("%s\n", (*out).c_str());
    if ((*out).rfind("ok", 0) == 0) {
        (*out).erase(0,3);
    } else {
        (*out).erase(0,4);
        (*out).erase(0,1).pop_back();
        return 1;
    }
    return 0;
}

string RequestMsg::toBase64(string pass) {
    string base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    vector<char> char_array(pass.begin(), pass.end());

    string binaryStr, padding, final;

    for (const char &c: char_array)
        binaryStr += bitset<8>(c).to_string();

    int lenght_bin = binaryStr.length();
    if((lenght_bin % 3) == 1)
        padding += "=";
    else if ((lenght_bin % 3) == 2)
        padding += "==";

    while ((lenght_bin = binaryStr.length()) % 6 != 0)
        binaryStr += "0";

    for (int i = 0; i < lenght_bin; i+=6)
        final += base64[stoi("00" + binaryStr.substr(i, 6), nullptr, 2)];

    return final += padding;
    
}

string RequestMsg::getToken() {
    string filename = "login-token.txt";
    fstream tokenFile;
    string readToken;

	tokenFile.open(filename, ios::in);
	if (!tokenFile) {
		cout << "No such file";
	}
	else {
        char ch;
		while (1) {
			tokenFile >> ch;
			if (tokenFile.eof())
				break;
			readToken += ch;
		}
	}
	tokenFile.close();
    
    return readToken;
}

int RequestMsg::createToken(string token) {
    string filename = "login-token.txt";
    fstream tokenFile;

    tokenFile.open(filename, ios::out);
    if (!tokenFile) {
		cout << "File not created!\n";
        return 1;
	}
	else {
		cout << "File created successfully!\n";
        tokenFile << token;
	}
    tokenFile.close();
    return 0;
}

RequestMsg::RequestMsg(string request, int numberOfArgs): request(request), numberOfArgs(numberOfArgs) {

}

string RequestMsg::buildString(vector<string> commnadArgs) {
    return "";
}

int RequestMsg::handleOutput(string out) {
    return 0;
}

int RequestMsg::getNumArg() {
    return RequestMsg::numberOfArgs;
}

Register::Register(): RequestMsg("register", 2) {
    
}

string Register::buildString(vector<string> commnadArgs) {
    string convPass = RequestMsg::toBase64(commnadArgs[1]);
    string builtStr = "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
    return builtStr;
}

int Register::handleOutput(string out) {
    int retCode = RequestMsg::resultParse(&out);

    RequestMsg::printResult(&out, retCode);

    return 0;
}

Login::Login(): RequestMsg("login", 2) {
    
}

string Login::buildString(vector<string> commnadArgs) {  
    string convPass = RequestMsg::toBase64(commnadArgs[1]);
    string builtStr = "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
    return builtStr;
}

int Login::handleOutput(string out) {
    int retCode = RequestMsg::resultParse(&out);
    string response = out;
    if (retCode == 0) {
        response = out.substr(0, out.rfind("\" \"") + 1);
        response.erase(0,1).pop_back();
        createToken(out.substr(response.length() + 3));
    }
    RequestMsg::printResult(&response, retCode);

    return 0;
}

List::List(): RequestMsg("list", 0) {
    
}

string List::buildString(vector<string> commnadArgs) {
    string token = RequestMsg::getToken();
    string builtStr = "(" + request + " " + token + " " + commnadArgs[0] + ")";
    return builtStr;
}

int List::handleOutput(string out) {
    int result = RequestMsg::resultParse(&out);
    return 0;
}

Fetch::Fetch(): RequestMsg("fetch", 1) {
    
}

string Fetch::buildString(vector<string> commnadArgs) {
    string token = RequestMsg::getToken();
    string builtStr = "(" + request + " " + token + " \"" + commnadArgs[0] + "\")";
    return builtStr;
}

int Fetch::handleOutput(string out) {
    int result = RequestMsg::resultParse(&out);
    return 0;
}

Send::Send(): RequestMsg("send", 3) {
    
}

string Send::buildString(vector<string> commnadArgs) {
    string token = RequestMsg::getToken();
    string builtStr = "(" + request + " " + token + " \"" + commnadArgs[0] + "\" \"" + commnadArgs[1] + "\" \"" + commnadArgs[2] + "\")";
    return builtStr;
}

int Send::handleOutput(string out) {
    int result = RequestMsg::resultParse(&out);
    return 0;
}

Logout::Logout(): RequestMsg("logout", 0){
    
}

string Logout::buildString(vector<string> commnadArgs) {
    string token = RequestMsg::getToken();
    string builtStr = "(" + request + " " + token + ")";
    return builtStr;
}

int Logout::handleOutput(string out) {
    int result = RequestMsg::resultParse(&out);
    return 0;
}
