#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>
using namespace std;

class CommandParser {
    private:
        // bool commandIsValid;
        vector<string> commandTokens;
        bool nameTooLong(const string &name);
        void tokenize(const string &commandString);
        bool blockNumInRange(const string &blockNum);
        bool validFileSize(const string &fileSize);
    public:
        bool checkMount();
        bool validReadWrite();
        bool validFileOp();
        bool validBuffOp();
        bool validNoArgOp();
        bool validOneArgOp();
        bool validate();
        CommandParser();
        vector<string> parse(const string &commandString);
};