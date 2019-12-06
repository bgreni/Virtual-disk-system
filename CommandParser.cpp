#include "CommandParser.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include "Constants.hpp"
using namespace std;

/**
 * @brief default constructor
*/
CommandParser::CommandParser() {}

/**
 * @brief parse a command string into a vector of tokens
 * @param commandString - the original whole string of the command
 * @return vector<string> - a vector of the tokenized command
*/
vector<string> CommandParser::parse(const string &commandString) {
    tokenize(commandString);
    return commandTokens;
}

/**
 * @brief validate the current command is both syntactically and semantically correct
 * @return bool - true if the command is valid
*/
bool CommandParser::validate() {
    string command = commandTokens[0];
    // try and find the command name in the commandMap (SEE Constants.h)
    if (commandMap.find(command) == commandMap.end()) {
        return false;
    } else {
        // run the command specific check
        FunctionPointer p = commandMap[command];
        return (this->*p)();
    }
}

/////////////////////////////////////////////////
// Command Validators
/////////////////////////////////////////////////

/**
 * @brief validate a mount command
 * @return bool true if the command is valid
*/
bool CommandParser::checkMount() {
    commandTokens[1].erase(remove_if(commandTokens[1].begin(), commandTokens[1].end(), ::isspace), commandTokens[1].end());
    if (commandTokens.size() == ONE_ARG_COMMAND) {
        return true;
    }
    return false;
}

/**
 * @brief validate a read or write command
 * @return bool true if the command is valid
*/
bool CommandParser::validReadWrite() {
    if (commandTokens.size() == TWO_ARG_COMMAND && !nameTooLong(commandTokens[1]) && validFileSize(commandTokens[2])) {
        return true;
    }
    return false;
}

/**
 * @brief used to validate resize command
 * @return bool true if the command is valid
*/
bool CommandParser::validFileOp() {
    commandTokens[1].erase(remove_if(commandTokens[1].begin(), commandTokens[1].end(), ::isspace), commandTokens[1].end());
    if (commandTokens.size() == TWO_ARG_COMMAND && (!nameTooLong(commandTokens[1])) && blockNumInRange(commandTokens[2])) {
        return true;
    }
    return false;
}

/**
 * @brief validate a buffer command
 * @return bool true if the command is valid
*/
bool CommandParser::validBuffOp() {
    if (commandTokens.size() == ONE_ARG_COMMAND && commandTokens[1].length() <= MAX_BUFF_LEN && commandTokens[1].length() > 0) {
        return true;
    }
    return false;
}

/**
 * @brief used for LS and Defrag commands
 * @return bool true if the command is valid
*/
bool CommandParser::validNoArgOp() {
    if (commandTokens.size() == NO_ARG_COMMAND) {
        return true;
    }
    return false;
}

/**
 * @brief Used for CD and Delete commands
 * @return bool true if the command is valid
*/
bool CommandParser::validOneArgOp() {
    commandTokens[1].erase(remove_if(commandTokens[1].begin(), commandTokens[1].end(), ::isspace), commandTokens[1].end());
    if (commandTokens.size() == ONE_ARG_COMMAND && !nameTooLong(commandTokens[1])) {
        return true;
    }
    return false;
}

/**
 * @brief validate a create command
 * @return bool true if the command is valid
*/
bool CommandParser::validCreateOp() {
    commandTokens[1].erase(remove_if(commandTokens[1].begin(), commandTokens[1].end(), ::isspace), commandTokens[1].end());
    if (commandTokens.size() == TWO_ARG_COMMAND && (!nameTooLong(commandTokens[1])) && validCreateSize(commandTokens[2])) {
        return true;
    }
    return false;
}



/////////////////////////////////////////////////
// Other Helpers
/////////////////////////////////////////////////

/**
 * @brief tokenizes a command and removes spaces
*/
void CommandParser::tokenize(const string &commandString) {
    commandTokens.clear();
    stringstream ss(commandString);
    string token;
    while (ss.good()) {
        ss >> token;
        commandTokens.push_back(token);
        if (token == BUFFER) {
            getline(ss, token);
            if (token != commandTokens[0]) {
                commandTokens.push_back(token);
            }
            break;
        }
    }
}

/**
 * @brief checks if a given string name is longer than the limit of 5 characters
 * @return bool - true if the string is no more than 5 characters
*/
bool CommandParser::nameTooLong(const string &name) {
    return name.length() > MAX_NAME_LEN;
}

/**
 * @brief check if a given block number is within the valid range
 * @return bool - true is the block number is valid
*/
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

/**
 * @brief checks if a given file size is valid
 * @return bool - true if the file size is valid
*/
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

/**
 * @brief checks if the size of new file is valid
 * @return bool - true if the file size is valid
*/
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
