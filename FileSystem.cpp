#include "FileSystem.hpp"
#include <iostream>  
#include <map>
#include <vector>
#include<cstdio>
using namespace std;

FileSystem::FileSystem() {
    diskIsMounted = false;
    superBlock = SuperBlock();
}


///////////////////////////////////////////////////
// Main File System Commands
///////////////////////////////////////////////////


void FileSystem::fs_mount(const string &new_disk_name) {
    cout << "Mounting the drive: " << new_disk_name << endl;
    
    diskFile.open(new_disk_name, ios::in | ios::out | ios::binary);
    if (!diskFile.is_open()) {
        cerr << "Error: Cannot find disk: " << new_disk_name << endl;
    }


    diskFile.read(reinterpret_cast<char*>(&superBlock), sizeof(SuperBlock));
    superBlock.fixFreeBlockList();
    // cout << superBlock.free_block_list << endl;
    
    int consistencyErrCode = superBlock.checkConsistency();

    if (consistencyErrCode != 0) {
        cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: " << consistencyErrCode << ")" << endl;
    } else {
        diskIsMounted = true;
        currentDiskName = new_disk_name;
        currentDirectory = ROOT_DIR;
    }
}

void FileSystem::fs_create(const string &name, int size) {
    int freeIndex = superBlock.findFreeNode();
    if (freeIndex == -1) {
        cerr << "Error: Superblock in disk " << currentDiskName << " is full, cannot create " << name << endl;
        return;
    }
    if (!superBlock.validNewName(name, currentDirectory)) {
        cerr << "Error: File or directory " << name << " already exists" << endl;
        return;
    }
    int startBlock = 0;
    if (size != 0) {
        startBlock = superBlock.findContigBlock(size);
        if (startBlock == -1) {
            cerr << "Error: cannot allocate " << size << " on " <<currentDiskName << endl;
            return;
        }
    }
    Inode newNode = Inode(name, size, startBlock, currentDirectory);
    superBlock.setNode(newNode, freeIndex);
    superBlock.setBlock(startBlock, startBlock + (size - 1));
    superBlock.buildDirectoryMap();
}


void FileSystem::fs_delete(const string &name) {
    superBlock.deleteNode(name, currentDirectory);
}

void FileSystem::fs_read(const string &name, int block_num) {
    uint8_t index = superBlock.getInodeIndex(name, currentDirectory);
    Inode node = superBlock.getNode(index);
    if (index == INVALID_NODE_NUM || !node.isAFile()) {
        cerr << "Error: " << name << " does not exist" << endl;
        return;
    }
    int size = node.getUsedSize();
    if (block_num < 0 || block_num > size - 1) {
        cerr << name << " does not have block " << block_num << endl;
        return;
    }
    int start = node.getStartBlock();
    int blockToRead = (start+block_num) * BLOCK_SIZE;
    diskFile.seekg(blockToRead);
    diskFile.read((char*)buffer, MAX_BUFF_LEN);

    cout << "CONTENTS OF BUFFER: " << (char*)buffer << endl;
}

void FileSystem::fs_write(const string &name, int block_num) {
    uint8_t index = superBlock.getInodeIndex(name, currentDirectory);
    Inode node = superBlock.getNode(index);
    if (index == INVALID_NODE_NUM || !node.isAFile()) {
        cerr << "Error: " << name << " does not exist" << endl;
        return;
    }
    int size = node.getUsedSize();
    if (block_num < 0 || block_num > size - 1) {
        cerr << name << " does not have block " << block_num << endl;
        return;
    }
    int start = node.getStartBlock();
    int blockToRead = (start+block_num) * BLOCK_SIZE;
    diskFile.seekg(blockToRead);
    diskFile.write((char*)buffer, MAX_BUFF_LEN);
}

void FileSystem::fs_buff(uint8_t buff[MAX_BUFF_LEN]) {
    clearBuffer();
    for (int i = 0; i < MAX_BUFF_LEN; i++) {
        buffer[i] = buff[i];
    }
}


// just sneaking these in here

void printDir(const char* name, const int numChildren) {
    // using printf so its easier to get the formating right
    printf("%-5s %3d\n",name, numChildren);
}

void printFile(const char* name, const int size) {
    printf("%-5s %3d KB\n", name, size);
}

void FileSystem::fs_ls(void) {
    cout << endl;
    map<uint8_t, vector<uint8_t>> directoryStructure = superBlock.getDirectoryMap();
    // print cwd
    vector<uint8_t> dirContents = directoryStructure[currentDirectory];
    int size = dirContents.size() + 2;
    printDir(CUR_DIR_STRING.c_str(), size);
    if (currentDirectory == ROOT_DIR) {
        printDir(PARENT_DIR_STRING.c_str(), size);
    } else {
        Inode node = superBlock.getNode(currentDirectory);
        uint8_t parent = node.getParent();
        vector<uint8_t> parentContents = directoryStructure[parent];
        int parentSize = parentContents.size() + 2;
        printDir(PARENT_DIR_STRING.c_str(), parentSize);
    }

    for (auto index : dirContents) {
        Inode node = superBlock.getNode(index);
        if (node.isAFile()) {
            printFile(node.getName().c_str(), node.getUsedSize());
        } else {
            vector<uint8_t> childContents = directoryStructure[index];
            int childSize = childContents.size() + 2;
            printDir(node.getName().c_str(), childSize);
        }
    }
}

void FileSystem::fs_resize(const string &name, int new_size) {
    uint8_t index = superBlock.getInodeIndex(name, currentDirectory);
    Inode node = superBlock.getNode(index);
    if (new_size > MAX_BLOCK_NUM) {
        cerr << "Error: File " << node.getName() << " cannot be expanded to size " << new_size << endl;
        return;
    }
    if (new_size == node.getUsedSize()) return;
    if (index == INVALID_NODE_NUM || !node.isAFile()) {
        cerr << "Error: " << name << " does not exist" << endl;
        return;
    }
    if (new_size < node.getUsedSize()) {
        shrinkBlock(index, node, new_size);
    } else {
        growBlock(index, node, new_size);
    }
    cout << superBlock.free_block_list << endl;
}

void FileSystem::shrinkBlock(uint8_t index, Inode &node, int newSize) {
    int oldEnd = node.getEndIndex();
    node.setUsedSize(newSize);
    int newEnd = node.getEndIndex();
    superBlock.clearBlock(newEnd + 1, oldEnd);

    uint8_t buf[BLOCK_SIZE];
    // making sure everything is zeroed out
    for (int i = 0; i < BLOCK_SIZE; i++) {
        buf[i] = 0;
    }

    int start = oldEnd * BLOCK_SIZE;
    for (int i = oldEnd; i >= newEnd + 1; i--) {
        diskFile.seekg(start);
        diskFile.write((char*)buf, BLOCK_SIZE);
    }
    superBlock.setNode(node, index);
}


void FileSystem::growBlock(uint8_t index, Inode &node, int newSize) {
    int oldEnd = node.getEndIndex();
    node.setUsedSize(newSize);
    int newEnd = node.getEndIndex();
    if (superBlock.isFreeBlock(oldEnd + 1, newEnd)) {
        superBlock.setBlock(oldEnd + 1, newEnd);
    } else {
        int newStart = superBlock.findContigBlock(newSize);
        if (newStart == -1) {
            cerr << "Error: File " << node.getName() << " cannot be expanded to size " << newSize << endl;
            return;
        }
        Inode newNode = node;
        superBlock.clearBlock(node.getStartBlock(), oldEnd);
        node.setStartBlock(newStart);
        superBlock.setBlock(newStart, newNode.getEndIndex());
        copyBlocks(node, newNode);
        superBlock.setNode(newNode, index);
    }
}

void FileSystem::copyBlocks(Inode oldNode, Inode newNode) {
    // use a new buffer in case the global buffer still has something we need
    uint8_t buf[1024];
    int oldNodePos = oldNode.getStartBlock() * BLOCK_SIZE;
    int newNodePos = newNode.getStartBlock() * BLOCK_SIZE;
    for (int i = 0; i < oldNode.getUsedSize(); i++) {
        diskFile.seekg(oldNodePos);
        diskFile.read((char*)buf, BLOCK_SIZE);
        diskFile.seekg(newNodePos);
        diskFile.write((char*)buf, BLOCK_SIZE);

        oldNodePos += BLOCK_SIZE;
        newNodePos += BLOCK_SIZE;
    }
}


void FileSystem::fs_cd(string &name) {
    if (name == CUR_DIR_STRING) {
        return;
    }
    else if (name == PARENT_DIR_STRING) {
        if (currentDirectory == ROOT_DIR) {
            return;
        }
        currentDirectory = superBlock.getNode(currentDirectory).getParent();
    } else {
        int index = superBlock.getInodeIndex(name, currentDirectory);
        if (index == INVALID_NODE_NUM) {
            cerr << "Error: Directory " << name << " does not exist" << endl;
            return; 
        }
        currentDirectory = index;
    }
}


///////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////

bool FileSystem::openInputFile(const string &filename) {
    inputFile.open(filename, ios::in);
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

void FileSystem::runCommand(vector<string> tokens) {

    string command = tokens[0];

    if (!diskIsMounted && command != MOUNT) {
        cerr << "Error: No file system is mounted" << endl;
    }
    
    if (command == MOUNT) {
        fs_mount(tokens[1]);
    }
    else if (command == CREATE) {
        int size = stoi(tokens[2]);
        fs_create(tokens[1], size);
    }
    else if (command == DELETE) {
        fs_delete(tokens[1]);
    }
    else if (command == READ) {
        int blockNum = stoi(tokens[2]);
        fs_read(tokens[1], blockNum);
    }
    else if (command == WRITE) {
        int blockNum = stoi(tokens[2]);
        fs_write(tokens[1], blockNum);
    }
    else if (command == BUFFER) {
        fs_buff((uint8_t*)tokens[1].c_str());
    }
    else if (command == LS) {
        fs_ls();
    }
    else if (command == RESIZE) {
        int newSize = stoi(tokens[2]);
        fs_resize(tokens[1], newSize);
    }
    else if (command == CD) {
        fs_cd(tokens[1]);
    }
}


void FileSystem::close() {
    diskFile.close();
    inputFile.close();
}


void FileSystem::clearBuffer() {
    for (int i = 0; i < MAX_BUFF_LEN; i++) {
        buffer[i] = 0;
    }
}





