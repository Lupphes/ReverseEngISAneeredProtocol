#include "requestmsg.hpp"

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
        return 1;
    }
    (*out).erase(0,1).pop_back();
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

int RequestMsg::getToken(string* token) {
    string filename = "login-token.txt";
    fstream tokenFile;

	tokenFile.open(filename, ios::in);
	if (!tokenFile) {
		printf("Not logged in\n");
        return 1;
	}
	else {
        char ch;
		while (1) {
			tokenFile >> ch;
			if (tokenFile.eof())
				break;
			(*token) += ch;
		}
	}
	tokenFile.close();
    
    return 0;
}

int RequestMsg::createToken(string token) {
    string filename = "login-token.txt";
    fstream tokenFile;

    tokenFile.open(filename, ios::out);
    if (!tokenFile) {
		printf("File not created!\n");
        return 1;
	}
	else {
        tokenFile << token;
	}
    tokenFile.close();
    return 0;
}

int RequestMsg::removeToken() {
    int status;
    string filename = "login-token.txt";
    status = remove(filename.c_str());
    if (status == 0)
        printf("File deleted successfully!\n");
    else
        printf("File not deleted!\n");
    return status;
}

RequestMsg::RequestMsg(string request, int numberOfArgs): request(request), numberOfArgs(numberOfArgs) {

}

string RequestMsg::buildString(vector<string> commnadArgs) {
    return "";
}

int RequestMsg::handleOutput(string &out) {
    cout << "handling input from base class" << endl;
    return 0;
}

int RequestMsg::getNumArg() {
    return RequestMsg::numberOfArgs;
}

void RequestMsg::getError() {}

Register::Register(): RequestMsg("register", 2) {
    
}

string Register::buildString(vector<string> commnadArgs) {
    string convPass = RequestMsg::toBase64(commnadArgs[1]);
    string builtStr = "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
    return builtStr;
}

int Register::handleOutput(string &out) {
    int retCode = RequestMsg::resultParse(&out);
    if (retCode == 0) {
        out.erase(0,1).pop_back();
    }
    RequestMsg::printResult(&out, retCode);
    return 0;
}

void Register::getError() {
    cout << "register <username> <password>\n";
}

Login::Login(): RequestMsg("login", 2) {
    
}

string Login::buildString(vector<string> commnadArgs) {  
    string convPass = RequestMsg::toBase64(commnadArgs[1]);
    string builtStr = "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
    return builtStr;
}

int Login::handleOutput(string &out) {
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

void Login::getError() {
    cout << "login <username> <password>\n";
}

List::List(): RequestMsg("list", 0) {
    
}

string List::buildString(vector<string> commnadArgs) {
    string token;
    if (RequestMsg::getToken(&token) != 0) {
        return "";
    }
    string builtStr = "(" + request + " " + token + ")";
    return builtStr;
}

int List::handleOutput(string &out) {
    int retCode = RequestMsg::resultParse(&out);
    string response = out;

    if (retCode == 0) {
        response = response.substr(2, response.length() - 1);
        string message, word;
        string wordDelimiter = "\" \"";
        string msgDelimiter = ") (";
        string result = "\n";
        int pos = 0;
        int pos2 = 0; 
        int index = 1;

        while ((pos = response.find(msgDelimiter)) != string::npos) {
            message = response.substr(0, pos);
            message = message.erase(0, 2);

            while ((pos2 = message.find(wordDelimiter)) != string::npos) {
                word = message.substr(1, pos2 - 1);
                message.erase(0, pos2 + wordDelimiter.length());
                message = message.substr(0, message.length() - 1);
                
                result += to_string(index) + ":\n  From: " + word.c_str() + "\n  Subject: " + message.c_str() + "\n";
                index++;
            }    
            response.erase(0, pos + msgDelimiter.length());
        }
        response = result;
    }
    RequestMsg::printResult(&response, retCode);
    return 0;
}

void List::getError() {
    cout << "list\n";
}

Fetch::Fetch(): RequestMsg("fetch", 1) {
    
}

string Fetch::buildString(vector<string> commnadArgs) {
    string token;
    if (RequestMsg::getToken(&token) != 0) {
        return "";
    }
    string builtStr = "(" + request + " " + token + " " + commnadArgs[0] + ")";
    return builtStr;
}

int Fetch::handleOutput(string &out) {
    int retCode = RequestMsg::resultParse(&out);
    string response = out;      
    if (retCode == 0) {
        string from, subject, msg;
        string delimiter = "\" \"";
        array<string, 3> t;

        response = response.substr(1, response.length() - 2);

        for (int i = 0; i < t.size() - 1; i++) {
            t[i] = response.substr(i, response.find(delimiter) + 1); 
            response.erase(0, t[i].length());
        }
        t.back() = response.erase(0,1);

        response = "\n\nFrom: " + t[0].substr(1, t[0].length() - 2) + "\nSubject: " + t[1].substr(1, t[1].length() - 3) + "\n\n" + t[2].substr(1, t[2].length() - 2);
    }
    RequestMsg::printResult(&response, retCode);
    return 0;
}

void Fetch::getError() {
    cout << "fetch <id>\n";
}

Send::Send(): RequestMsg("send", 3) {
    
}

string Send::buildString(vector<string> commnadArgs) {
    string token;
    if (RequestMsg::getToken(&token) != 0) {
        return "";
    }
    string builtStr = "(" + request + " " + token + " \"" + commnadArgs[0] + "\" \"" + commnadArgs[1] + "\" \"" + commnadArgs[2] + "\")";
    return builtStr;
}

int Send::handleOutput(string &out) {
    int retCode = RequestMsg::resultParse(&out);
    if (retCode == 0) {
        out.erase(0,1).pop_back();
    }
    RequestMsg::printResult(&out, retCode);
    return 0;
}

void Send::getError() {
    cout << "send <recipient> <subject> <body>\n";
}

Logout::Logout(): RequestMsg("logout", 0){
    
}

string Logout::buildString(vector<string> commnadArgs) {
    string token;
    if (RequestMsg::getToken(&token) != 0) {
        return "";
    }
    string builtStr = "(" + request + " " + token + ")";
    return builtStr;
}

int Logout::handleOutput(string &out) {
    int retCode = RequestMsg::resultParse(&out);
    int code = removeToken();
    if (retCode == 0) {
        out.erase(0,1).pop_back();
    }
    RequestMsg::printResult(&out, code + retCode);
    return 0;
}

void Logout::getError() {
    cout << "logout\n";
}
