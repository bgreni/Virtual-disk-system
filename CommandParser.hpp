#pragma once

#include <vector>
#include <string>
using namespace std;

class CommandParser {
    private:
        // bool commandIsValid;
        vector<string> commandTokens;                       // vector of tokens for the current command being parsed
        bool nameTooLong(const string &name);               // checks if a filename is longer than 5 characters
        void tokenize(const string &commandString);         // tokenize the initial command string
        bool blockNumInRange(const string &blockNum);       // checks if a block number is valid
        bool validFileSize(const string &fileSize);         // checks if a file size is valid
        bool validCreateSize(const string &size);           // checks if the size of a create file command is valid
    public:
        bool checkMount();                                  // return true if commandTokens represents a valid mount command
        bool validReadWrite();                              // return true if valid read/write command
        bool validFileOp();                                 // retrun true if valid file operation
        bool validBuffOp();                                 // return true if valid buffer operation
        bool validNoArgOp();                                // return true if valid no arg operation
        bool validOneArgOp();                               // return true if valid one arg operation
        bool validCreateOp();                               // retun true if valid create operation
        bool validate();                                    // return true if command is valid
        CommandParser();                                    // default constructor
        vector<string> parse(const string &commandString);  // parse a command string
};