#include "requestmsg.hpp"
#include "errorcodes.hpp"

#include <bitset>
#include <vector>
#include <fstream>
#include <regex>

#define filename "login-token"

void RequestMsg::printResult(string &result, int code) {
    if (code == 0) cout << "SUCCESS: " << result << "\n";
    else cout << "ERROR: " << result << "\n";
}

int RequestMsg::resultParse(string &out) {
    regex regPacketOk("^\\((ok ){1}.*\\)$");
    regex regPacketErr("^\\((err ){1}.*\\)$");
    
    cout << out << "\n";
    if (regex_match(out, regPacketOk)) {
        out = out.substr(4, out.length() - 5);
    } else if (regex_match(out, regPacketErr)) {
        out = out.substr(5, out.length() - 6);
        return returnCodes::SERVER_ERR;
    } else {
        cerr << "Packet is in invalid format\n";
        return returnCodes::ERR_PACKET_INVALID_FORMAT;
    }
    return returnCodes::SUCCESS;
}

int RequestMsg::isNumber(string number) {
    for (char const &c : number) {
        if (isdigit(c) == 0) {
            cerr << "id " << number << " is not a number\n";
            return returnCodes::ERR_ID_NOT_NUMBER;
        }
    }
    return returnCodes::SUCCESS;
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

int RequestMsg::getToken(string &token) {
    fstream tokenFile;

	tokenFile.open(filename, ios::in);
	if (not tokenFile) {
		cout << "Not logged in\n";
        return returnCodes::ERR_CANT_OPEN_FILE;
	}
	else while(!tokenFile.eof()) getline(tokenFile, token);

	tokenFile.close();
    
    return returnCodes::SUCCESS;
}

int RequestMsg::createToken(string token) {
    fstream tokenFile;

    tokenFile.open(filename, ios::out);
    if (not tokenFile) {
        cerr << "Error while saving the token\n";
        return returnCodes::ERR_CANT_CREATE_FILE;
	}
	else tokenFile << token;

    tokenFile.close();
    return returnCodes::SUCCESS;
}

int RequestMsg::removeToken() {  
    int status = remove(filename);
    if (status != 0) {
        cerr << "Your token coundn't be deleted";
        return returnCodes::ERR_CANT_DELETE_FILE;
    }
    return returnCodes::SUCCESS;
}

int RequestMsg::escapeChars(vector<string> &input) {
    for (auto & item : input) {
        string temp;
        for(char& c : item) {
            switch (c) {
                case '\n':
                    temp += "\\n";
                    continue;
                case '\t':
                    temp += "\\t";
                    continue;
                case '"':
                case '\\':
                    temp += "\\";
                    break;
                default:
                    break;
            }
            temp += c;
        }
        item = temp;
    }

    // DEBUG: Print escaped strings
    for (auto & item : input) {
        // DEBUG: 
        cout << item << endl;
    }
    
    return returnCodes::SUCCESS;
}

int RequestMsg::unescapeChars(string &input) {
    string temp; 
    int length = input.length();
    for (int i = 0; i < length; ++i) {
        
        if (input[i] == '\\' and i+1 <= length) {
            switch (input[i+1]) {
                case 'n':
                    temp += "\n";
                    i += 1;
                    break;
                case 't':
                    temp += "\t";
                    i += 1;
                    break;
                case '\\':
                case '"':
                    temp += input[i+1];
                    i += 1;
                    break;
                default:
                    break;
            }
            continue;
        }
        temp += input[i];
    }
    input = temp;
    // DEBUG:
    cout << input << endl;
    return returnCodes::SUCCESS;
}

RequestMsg::RequestMsg(string request, int numberOfArgs): request(request), numberOfArgs(numberOfArgs) { }

int RequestMsg::getNumArg() {
    return this->numberOfArgs;
}

Register::Register(): RequestMsg("register", 2) {}

string Register::buildString(vector<string> commnadArgs) {
    escapeChars(commnadArgs);
    string convPass = toBase64(commnadArgs[1]);
    return "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
}

int Register::handleOutput(string &out) {
    int retCode = resultParse(out);
    if (retCode == returnCodes::SUCCESS) {
        out = out.substr(1, out.size() - 2);
    }
    unescapeChars(out);
    printResult(out, retCode);
    return 0;
}

void Register::getError() {
    cout << "register <username> <password>\n";
}

Login::Login(): RequestMsg("login", 2) { }

string Login::buildString(vector<string> commnadArgs) {  
    escapeChars(commnadArgs);
    string convPass = toBase64(commnadArgs[1]);
    return "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
}

int Login::handleOutput(string &out) {
    int retCode = resultParse(out);
    if (retCode == returnCodes::SUCCESS) {
        vector<string> matches;
        splitByRegex(out, matches);
        out = matches.at(0).substr(1, matches.at(0).size() - 2);
        createToken(matches.at(1));
    }
    unescapeChars(out);
    printResult(out, retCode);

    return 0;
}

int RequestMsg::splitByRegex(string str, vector<string> &matches) { 
    string raw_str = R"(\"(\\.|[^\"])*\")";
    regex regSpace(raw_str);
    regex regBrac("");

   std::smatch match;
    while (std::regex_search(str, match, regSpace)) {
        cout << "[" << match.str() << "]" << '\n';
        matches.push_back(match.str());
        str = match.suffix().str();
    }

    return returnCodes::SUCCESS;
}

void Login::getError() {
    cout << "login <username> <password>\n";
}

List::List(): RequestMsg("list", 0) { }

string List::buildString(vector<string> commnadArgs) {
    string token;
    if (getToken(token) != 0) {
        return "";
    }
    return "(" + request + " " + token + ")";
}

int List::handleOutput(string &out) {
    cout << out << endl;
    int retCode = resultParse(out);
    string response = out;

    if (retCode == 0) {
        response = response.substr(2, response.length() - 1);
        string message, word;
        string wordDelimiter = "\" \"";
        string msgDelimiter = ") (";
        string result = "\n";
        size_t pos = 0;
        size_t pos2 = 0; 
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
    RequestMsg::unescapeChars(response);
    RequestMsg::printResult(response, retCode);
    return 0;
}

void List::getError() {
    cout << "list\n";
}

Fetch::Fetch(): RequestMsg("fetch", 1) { }

string Fetch::buildString(vector<string> commnadArgs) {
    if (isNumber(commnadArgs[0]) != 0) {
        return "";
    }
    string token;
    if (getToken(token) != 0) {
        return "";
    }
    return "(" + request + " " + token + " " + commnadArgs[0] + ")";
}

int Fetch::handleOutput(string &out) {
    cout << out << endl;
    int retCodeParse = resultParse(out);
    if (retCodeParse == 0) {
        vector<string> matches;
        splitByRegex(out, matches);
        // string from, subject, msg;
        // string delimiter = "\" \"";
        // array<string, 3> t;
        // string response = out;

        // response = response.substr(1, response.length() - 2);

        // for (size_t i = 0; i < t.size() - 1; i++) {
        //     t[i] = response.substr(i, response.find(delimiter) + 1); 
        //     response.erase(0, t[i].length());
        // }
        // t.back() = response.erase(0,1);

        // string uno = t[0].substr(1, t[0].length() - 2);
        // string dos = t[1].substr(1, t[1].length() - 3);
        // string tres = t[2].substr(1, t[2].length() - 2);

        string uno, dos, tres;
        unescapeChars(uno);
        unescapeChars(dos);
        unescapeChars(tres);

        uno = matches.at(0);
        dos = matches.at(1);
        tres = matches.at(2);

        uno.substr(1, matches.at(0).size() - 2);
        dos.substr(1, matches.at(1).size() - 2);
        tres.substr(1, matches.at(2).size() - 2);



        

        out = "\n\nFrom: " + uno + "\nSubject: " + dos + "\n\n" + tres;
    }
    printResult(out, retCodeParse);
    return 0;
}

void Fetch::getError() {
    cout << "fetch <id>\n";
}

Send::Send(): RequestMsg("send", 3) { }

string Send::buildString(vector<string> commnadArgs) {
    escapeChars(commnadArgs);
    string token;
    if (getToken(token) != returnCodes::SUCCESS) {
        return "";
    }
    return "(" + request + " " + token + " \"" + commnadArgs[0] + "\" \"" + commnadArgs[1] + "\" \"" + commnadArgs[2] + "\")";
}

int Send::handleOutput(string &out) {
    int retCodeParse = resultParse(out);
    if (retCodeParse == returnCodes::SUCCESS) {
        out = out.substr(1, out.size() - 2);
    }
    unescapeChars(out);
    printResult(out, retCodeParse);
    return 0;
}

void Send::getError() {
    cout << "send <recipient> <subject> <body>\n";
}

Logout::Logout(): RequestMsg("logout", 0) { }

string Logout::buildString(vector<string> commnadArgs) {
    string token;
    if (getToken(token) != 0) {
        return "";
    }
    return "(" + request + " " + token + ")";
}

int Logout::handleOutput(string &out) {
    int retCodeParse = resultParse(out);
    int retCodeToken = removeToken();
    if (retCodeParse == returnCodes::SUCCESS and retCodeToken == returnCodes::SUCCESS)
        out = out.substr(1, out.size() - 2);
    printResult(out, retCodeParse + retCodeToken);
    return 0;
}

void Logout::getError() {
    cout << "logout\n";
}