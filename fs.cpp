#include "FileSystem.hpp"
#include <iostream>
#include "CommandParser.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "No instruction file was provided" << endl;
        return 1;
    }

    fstream err("stdr.txt");
    cerr.rdbuf(err.rdbuf());

    string filename(argv[1]);
    cout << filename << endl;
    
    FileSystem fs = FileSystem();
    CommandParser parser = CommandParser();
    if (!fs.openInputFile(filename)) {
        return 1;
    }
    queue<string> commandQueue = fs.readCommands();

    cout << commandQueue.size() << endl;
    int i = 1;
    while (!commandQueue.empty()) {
        vector<string> tokens = parser.parse(commandQueue.front());
        if (parser.validate()) {
            fs.runCommand(tokens);
        } else {
            cerr << "Command Error: " << filename << ", " << i << endl;
        }
        i++;
        commandQueue.pop();
    }
    fs.close();
    err.close();
    return 0;
}