#include "handler.hpp"

int RequestHandler::buildClientString(int argc, char** argv, int &args_processed, string &builtString) {

    if (argv[args_processed] == 0 || this->request_dict.find(argv[args_processed]) == this->request_dict.end()) {
        cerr << "unknown command\n";
        return returnCodes::ERR_ARG;
    }

    this->command = this->request_dict.at(argv[args_processed]);

    if ((args_processed + this->command->getNumArg()) != (argc - 1)) {
        this->command->getError();
        return returnCodes::ERR_UNKNOWN_COMMAND;
    }
    
    vector<string> commnadArgs;
    for (int i = args_processed + 1; i < argc; i++) {
        commnadArgs.push_back(argv[i]);
    }

    string result;

    int retCode;
    try {
        if ((retCode = command->buildString(commnadArgs, result)) != 0)
            return retCode;
    } catch (const std::regex_error& e) {
        cerr << "Unexpected parsing error occured: " << e.what();
        return returnCodes::ERR_UNEXPECTED_REGEX_ERROR;
    } catch (const std::out_of_range& e) {
        cerr << "Unexpected parsing error occured: " << e.what();
        return returnCodes::ERR_UNEXPECTED_PARSING_ERROR;
    } catch (const std::exception& e) {
        cerr << "Unexpected error occured: " << e.what();
        return returnCodes::ERR_UNEXPECTED_ERROR;
    }

    builtString = result;

    return returnCodes::SUCCESS;
}

int RequestHandler::exchangeData(int &sock, string &builtString) {
    int sendCode, readCode;
    char buffer[bufferLenght] = {0};
    string recievedString;
    
    // DEBUG: Built string
    // cout << builtString << endl;
    sendCode = send(sock, builtString.c_str(), builtString.length(), 0);
    if (sendCode != -1) {
        while ((readCode = recv(sock, buffer, bufferLenght - 1, 0))) {
            if (readCode == -1) {
                cerr << "Response cannot be read\n";
                return returnCodes::ERR_RECV_UNSUCCESSFUL;
            }
                recievedString += buffer;
                bzero(buffer, bufferLenght);
        }
    } else {
        cerr << "Request cannot not be send\n";
        return returnCodes::ERR_SEND_UNSUCCESSFUL;
    }
    
    try {
        if (this->command->handleOutput(recievedString) != returnCodes::SUCCESS) {
            return returnCodes::ERR_INCORRECT_OUTPUT;
        }
    }
    catch (const std::regex_error& e) {
        cerr << "Unexpected parsing error occured: " << e.what();
        return returnCodes::ERR_UNEXPECTED_REGEX_ERROR;
    }
    catch (const std::out_of_range& e) {
        cerr << "Unexpected parsing error occured: " << e.what();
        return returnCodes::ERR_UNEXPECTED_PARSING_ERROR;
    }
    catch (const std::exception& e) {
        cerr << "Unexpected error occured: " << e.what();
        return returnCodes::ERR_UNEXPECTED_ERROR;
    }

    return returnCodes::SUCCESS;
}
