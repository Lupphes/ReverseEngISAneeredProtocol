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
        // DEBUG: Input of regex
        // cout << "Input is:" << item << endl;
        string temp;       
        for(char& c : item) {
            switch (c) {
                case '\n':  temp += "\\n";      break;
                case '\t':  temp += "\\t";      break;
                case '\\':  temp += "\\\\";     break;
                case '"':   temp += "\\\"";     break;
                default:    temp += c;          break;
            }
        }
        // DEBUG: Escaped of regex
        // cout << "Escaped is:" << temp << endl;

        item = temp;
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
    // cout << "Unescaped is: " << input << endl;
    return returnCodes::SUCCESS;
}

RequestMsg::RequestMsg(string request, int numberOfArgs): request(request), numberOfArgs(numberOfArgs) { }

int RequestMsg::getNumArg() {
    return this->numberOfArgs;
}

Register::Register(): RequestMsg("register", 2) {}

int Register::buildString(vector<string> commnadArgs, string &result) {
    escapeChars(commnadArgs);
    string convPass = toBase64(commnadArgs[1]);
    result = "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
    return returnCodes::SUCCESS;
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

int Login::buildString(vector<string> commnadArgs, string &result) {  
    escapeChars(commnadArgs);
    string convPass = toBase64(commnadArgs[1]);
    result = "(" + request + " \"" + commnadArgs[0] + "\" \"" + convPass + "\")";
    return returnCodes::SUCCESS;
}

int Login::handleOutput(string &out) {
    int retCode = resultParse(out);
    int retCodeParse;
    if (retCode == returnCodes::SUCCESS) {
        vector<string> matches;
        splitByRegex(out, matches);
        out = matches.at(0);
        if ((retCodeParse = createToken("\"" + matches.at(1) + "\"")) != returnCodes::SUCCESS) {
            return retCodeParse;
        }
    }
    unescapeChars(out);
    printResult(out, retCode);
    return returnCodes::SUCCESS;
}

int RequestMsg::splitByRegex(string str, vector<string> &matches) { 
    int length = str.length();
    string temp;
    bool flagMark = false;
    for (int i = 0; i < length; ++i) {
        if (str[i] == '"') {
            if (flagMark) {
                // DEBUG: Parsing
                // cout << "[" << temp << "]" << '\n';
                matches.push_back(temp);
                temp = "";
                flagMark = false;
            } else flagMark = true;
            continue;
        } else if (str[i] == '\\') {         
            temp += str[i];   
            i++;
        }
        if (flagMark) temp += str[i];     
    }

    // Previous regex parse
    // string raw_str = R"(\"(\\.|[^\"])*\")";
    // regex regSpace(raw_str);

    // std::smatch match;
 
    // while (std::regex_search(str, match, regSpace)) {
    //     // DEBUG: Regex
    //     // cout << "[" << match.str() << "]" << '\n';
    //     matches.push_back(match.str());
    //     str = match.suffix().str();
    // }

    return returnCodes::SUCCESS;
}

void Login::getError() {
    cout << "login <username> <password>\n";
}

List::List(): RequestMsg("list", 0) { }

int List::buildString(vector<string> commnadArgs, string &result) {
    string token;
    int retCodeToken;
    if ((retCodeToken = getToken(token)) != returnCodes::SUCCESS) {
        return retCodeToken;
    }
    result = "(" + request + " " + token + ")";
    return returnCodes::SUCCESS;
}

int List::handleOutput(string &out) {
    int retCode = resultParse(out);
    string response = out;

    if (retCode == returnCodes::SUCCESS) {
        vector<string> matches;
        splitByRegex(out, matches);
        string response, from, subject;

        for (uint64_t i = 0; i < matches.size(); i += 2) {
            from = matches.at(i);
            subject = matches.at(i+1);
            response += "\n" + to_string(((i + 1)/2)+1) + ":\n  From: " + from + "\n  Subject: " + subject;
        }
        out = response;
    }
    unescapeChars(out);
    printResult(out, retCode);
    return returnCodes::SUCCESS;
}

void List::getError() {
    cout << "list\n";
}

Fetch::Fetch(): RequestMsg("fetch", 1) { }

int Fetch::buildString(vector<string> commnadArgs, string &result) {
    int retCodeParse, retCodeToken;
    if ((retCodeParse = isNumber(commnadArgs[0])) != returnCodes::SUCCESS) {
        return retCodeParse;
    }
    string token;
    if ((retCodeToken = getToken(token)) != returnCodes::SUCCESS) {
        return retCodeToken;
    }
    result = "(" + request + " " + token + " " + commnadArgs[0] + ")";
    return returnCodes::SUCCESS;
}

int Fetch::handleOutput(string &out) {
    int retCodeParse = resultParse(out);
    if (retCodeParse == 0) {
        vector<string> matches;
        splitByRegex(out, matches);

        unescapeChars(matches.at(0));
        unescapeChars(matches.at(1));
        unescapeChars(matches.at(2));

        out = "\n\nFrom: " + matches.at(0) + "\nSubject: " + matches.at(1) + "\n\n" + matches.at(2);
    }
    printResult(out, retCodeParse);
    return returnCodes::SUCCESS;
}

void Fetch::getError() {
    cout << "fetch <id>\n";
}

Send::Send(): RequestMsg("send", 3) { }

int Send::buildString(vector<string> commnadArgs, string &result) {
    int retCodeToken;
    escapeChars(commnadArgs);
    string token;
    if ((retCodeToken = getToken(token)) != returnCodes::SUCCESS) {
        return retCodeToken;
    }
    result = "(" + request + " " + token + " \"" + commnadArgs[0] + "\" \"" + commnadArgs[1] + "\" \"" + commnadArgs[2] + "\")";
    return returnCodes::SUCCESS;
}

int Send::handleOutput(string &out) {
    int retCodeParse = resultParse(out);
    if (retCodeParse == returnCodes::SUCCESS) {
        out = out.substr(1, out.size() - 2);
    }
    unescapeChars(out);
    printResult(out, retCodeParse);
    return returnCodes::SUCCESS;
}

void Send::getError() {
    cout << "send <recipient> <subject> <body>\n";
}

Logout::Logout(): RequestMsg("logout", 0) { }

int Logout::buildString(vector<string> commnadArgs, string &result) {
    string token;
    int retCodeToken;
    if ((retCodeToken = getToken(token)) != returnCodes::SUCCESS) {
        return retCodeToken;
    }
    result = "(" + request + " " + token + ")";
    return returnCodes::SUCCESS;
}

int Logout::handleOutput(string &out) {
    int retCodeParse = resultParse(out);
    int retCodeToken = removeToken();
    if (retCodeParse == returnCodes::SUCCESS and retCodeToken == returnCodes::SUCCESS)
        out = out.substr(1, out.size() - 2);
    printResult(out, retCodeParse + retCodeToken);
    return returnCodes::SUCCESS;
}

void Logout::getError() {
    cout << "logout\n";
}
