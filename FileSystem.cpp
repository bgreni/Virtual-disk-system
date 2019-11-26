#include "FileSystem.hpp"
#include <iostream>  
#include "SuperBlock.hpp"
using namespace std;

FileSystem::FileSystem() {
    diskIsMounted = false;
}

bool FileSystem::openInputFile(const string &filename) {
    inputFile.open(filename);
    if (!inputFile.is_open()) {
        cerr << "Error: input file does not exist: "  << filename << endl;
        return false;
    }
    return true;
}

queue<string> FileSystem::readCommands() {
    queue<string> commandQueue;
    string s;
    while (getline(inputFile, s)) {
        commandQueue.push(s);
    }
    return commandQueue;
}

void FileSystem::fs_mount(const string &new_disk_name) {
    cout << "Mounting the drive" << endl;

    SuperBlock superBlock = SuperBlock();
    
    diskFile.open(new_disk_name, ios::in | ios::out | ios::binary);
    if (!diskFile.is_open()) {
        cerr << "Error: Cannot find disk: " << new_disk_name << endl;

    }

    // somehow the c-style way of doing this works better
    diskFile.read((char*)&superBlock, BLOCK_SIZE);

    cout << superBlock.free_block_list << endl;

    // int consistencyErrCode = superBlock.checkConsistency();

    // if (consistencyErrCode != 0) {
    //     cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: " << consistencyErrCode << ")" << endl;
    // } else {
    //     diskIsMounted = true;
    // }
    
    // cout << superBlock.free_block_list.to_string() << endl;

    // for (int i = 0; i < numNodes; i++) {
    //     if (strlen(superBlock.inode[i].name) != 0) {
    //         cout << "------------------------" << endl;
    //         cout << superBlock.inode[i].name << endl;
    //         cout << superBlock.inode[i].start_block << endl;
    //         cout << "------------------------" << endl;
    //     }
    // }
}

// TODO: MAKE IT RUN THE COMMANDS

void FileSystem::runCommand(vector<string> tokens) {

    string command = tokens[0];

    if (!diskIsMounted && command != MOUNT) {
        cerr << "Error: No file system is mounted" << endl;
    }
    
    if (command == MOUNT) {
        fs_mount(tokens[1]);
    }
}


void FileSystem::close() {
    diskFile.close();
    inputFile.close();
}





