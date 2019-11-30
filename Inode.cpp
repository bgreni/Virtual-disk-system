#include "Inode.hpp"
#include "Constants.hpp"
#include <iostream>
#include <sstream>

using namespace std;

Inode::Inode() {
    usedSize = 0;
    startBlock = 0;
    parent = 0;
    for (int i = 0; i < 5; i++) {
        name[i] = '\0';
    }
}

Inode::Inode(const string &name, int size, int startBlock, int parent) {
    setName(name);
    setInUse(true);
    setParent(parent);
    if (size == 0) {
        setIsFile(false);
        setUsedSize(0);
        setStartBlock(0);
    } else {
        setIsFile(true);
        setUsedSize(size);
        setStartBlock(startBlock);
    }
}

bool Inode::nodeInUse() {
    return (usedSize >> 7) == 1;
}

bool Inode::blockInNodeRange(int index) {
    return index >= startBlock && index <= getEndIndex();
}

int Inode::getEndIndex() {
    return (startBlock + getUsedSize()) - 1;
}

bool Inode::nodeIsClean() {
    return startBlock == 0 && usedSize == 0 && parent == 0 && !hasName();
}

bool Inode::hasName() {
    bool hasChar = false;
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        if (name[i] != 0) {
            hasChar = true;
        }
    }
    return hasChar;
}


bool Inode::isAFile() {
    return ((parent >> 7) == 0) && nodeInUse();
}

bool Inode::checkStartBlock() {
    return startBlock >= MIN_BLOCK_NUM && startBlock <= MAX_BLOCK_NUM;
}

bool Inode::checkDirectoryAttributes() {
    return startBlock == 0 && getUsedSize() == 0;
}

///////////////////////////////////////////////////
// Getters/Setters
///////////////////////////////////////////////////

string Inode::getName() {
    return string(name,5);
}

void Inode::setName(string newName) {
    strcpy(name, newName.c_str());
}

uint8_t Inode::getUsedSize() {
    return (usedSize & ALL_BUT_LAST_MASK);
}

void Inode::setUsedSize(uint8_t newSize) {
    int stateBit = usedSize & LAST_BIT_MASK;
    usedSize = newSize;
    usedSize |= stateBit;
}

uint8_t Inode::getStartBlock() {
    return startBlock;
}

void Inode::setStartBlock(uint8_t newStartBlock) {
    startBlock = newStartBlock;
}

uint8_t Inode::getParent() {
    return parent & ALL_BUT_LAST_MASK;
}

void Inode::setParent(uint8_t newParent) {
    int modeBit = parent & LAST_BIT_MASK;
    parent = newParent;
    parent |= modeBit;
}

void Inode::setInUse(bool inUse) {
    if (inUse) {
        usedSize |= LAST_BIT_MASK;
    } else {
        usedSize &= ALL_BUT_LAST_MASK;
    }
}

void Inode::setIsFile(bool isFile) {
    if (isFile) {
        parent &= ALL_BUT_LAST_MASK;
    } else {
        parent |= LAST_BIT_MASK;
    }
}


string Inode::str() {
 stringstream ss;
 ss << "-----------------------------" << endl;
 ss << "Used Size: " << (int)getUsedSize() << endl;
 ss << "Start Block: " << (int)getStartBlock() << endl;
 ss << "Name: " << getName() << endl;
 ss << "Is a File: " << isAFile() << endl;
 ss << "Is in use: " << nodeInUse() << endl;
 ss << "-----------------------------" << endl;
 return ss.str();
}