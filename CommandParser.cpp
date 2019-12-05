#include "CommandParser.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include "Constants.hpp"
using namespace std;

CommandParser::CommandParser() {}


vector<string> CommandParser::parse(const string &commandString) {
    tokenize(commandString);
    return commandTokens;
}


bool CommandParser::validate() {
    string command = commandTokens[0];
    // cout << command << " : ";
    if (commandMap.find(command) == commandMap.end()) {
        return false;
    } else {
        FunctionPointer p = commandMap[command];
        return (this->*p)();
    }
}

/////////////////////////////////////////////////
// Command Validators
/////////////////////////////////////////////////

bool CommandParser::checkMount() {
    if (commandTokens.size() == ONE_ARG_COMMAND) {
        return true;
    }
    return false;
}

bool CommandParser::validReadWrite() {
    if (commandTokens.size() == TWO_ARG_COMMAND && !nameTooLong(commandTokens[1]) && validFileSize(commandTokens[2])) {
        return true;
    }
    return false;
}

bool CommandParser::validFileOp() {
    if (commandTokens.size() == TWO_ARG_COMMAND && (!nameTooLong(commandTokens[1])) && blockNumInRange(commandTokens[2])) {
        return true;
    }
    return false;
}

bool CommandParser::validBuffOp() {
    if (commandTokens.size() == ONE_ARG_COMMAND && commandTokens[1].length() <= MAX_BUFF_LEN) {
        return true;
    }
    return false;
}


bool CommandParser::validNoArgOp() {
    if (commandTokens.size() == NO_ARG_COMMAND) {
        return true;
    }
    return false;
}

bool CommandParser::validOneArgOp() {
    if (commandTokens.size() == ONE_ARG_COMMAND && !nameTooLong(commandTokens[1])) {
        return true;
    }
    return false;
}

bool CommandParser::validCreateOp() {
    if (commandTokens.size() == TWO_ARG_COMMAND && (!nameTooLong(commandTokens[1])) && validCreateSize(commandTokens[2])) {
        return true;
    }
    return false;
}



/////////////////////////////////////////////////
// Other Helpers
/////////////////////////////////////////////////

void CommandParser::tokenize(const string &commandString) {
    commandTokens.clear();
    stringstream ss(commandString);
    string token;
    while (getline(ss, token, ' ')) {
        commandTokens.push_back(token);
    }
    
    // TODO: this crashes on linux
    // for(string s : commandTokens) {
    //     s.erase(remove_if(s.begin(), s.end(), ::isspace));
    // }
}

bool CommandParser::nameTooLong(const string &name) {
    return name.length() > MAX_NAME_LEN;
}

bool CommandParser::blockNumInRange(const string &blockNum) {
    size_t block = 0;
    try {
        block = stoi(blockNum);
    } catch (const invalid_argument&) {
        return false;
    }
    if (block >= MIN_BLOCK_NUM && block <= MAX_BLOCK_NUM) {
        return true;
    }
    return false;
}

bool CommandParser::validFileSize(const string &fileSize) {
    int size = 0;
    try {
        size = stoi(fileSize);
    } catch(const invalid_argument&) {
        return false;
    }
    if (size > -1 && (size_t)size <= MAX_BLOCK_NUM) {
        return true;
    }
    return false;
}

bool CommandParser::validCreateSize(const string &size) {
    int intSize = 0;
    try {
        intSize = stoi(size);
    } catch(const invalid_argument&) {
        return false;
    }
    if (intSize >= 0 && intSize <= 126) {
        return true;
    }
    return false;
}
