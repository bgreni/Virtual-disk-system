#include "CommandParser.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include "Constants.hpp"
using namespace std;

// TODO: implement the rest of the checks

CommandParser::CommandParser() {}


vector<string> CommandParser::parse(const string &commandString) {
    tokenize(commandString);
    return commandTokens;
}


bool CommandParser::validate() {
    string command = commandTokens[0];
    cout << command << " : ";
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

    for(string s : commandTokens) {
        s.erase(remove_if(s.begin(), s.end(), ::isspace));
    }
}

bool CommandParser::nameTooLong(const string &name) {
    return name.length() > MAX_NAME_LEN;
}

bool CommandParser::blockNumInRange(const string &blockNum) {
    int block = 0;
    try {
        block = stoi(blockNum);
    } catch (invalid_argument e) {
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
    } catch(invalid_argument e) {
        return false;
    }
    if (size > -1 && size <= MAX_BLOCK_NUM) {
        return true;
    }
    return false;
}
