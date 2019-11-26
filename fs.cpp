#include "FileSystem.hpp"
#include <iostream>
#include "CommandParser.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "No instruction file was provided" << endl;
        return 1;
    }

    string filename(argv[1]);
    cout << filename << endl;
    
    FileSystem fs = FileSystem();
    CommandParser parser = CommandParser();
    if (!fs.openInputFile(filename)) {
        return 1;
    }
    queue<string> commandQueue = fs.readCommands();

    cout << commandQueue.size() << endl;

    while (!commandQueue.empty()) {
        // cout << commandQueue.front() << endl;
        vector<string> tokens = parser.parse(commandQueue.front());
        // cout << "Command is valid: " << parser.validate() << endl;
        if (parser.validate()) {
            fs.runCommand(tokens);
        }
        commandQueue.pop();
    }
    fs.close();
    return 0;
}