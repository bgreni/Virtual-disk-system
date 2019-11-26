#include "Inode.hpp"
#include "Constants.hpp"

using namespace std;

Inode::Inode() {}

bool Inode::nodeInUse() {
    return (usedSize >> 7) == 1;
}

bool Inode::blockInNodeRange(int index) {
    return index >= startBlock && index <= startBlock + usedSize - 1;
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
    return (parent >> 7) == 0;
}

bool Inode::checkStartBlock() {
    return startBlock > MIN_BLOCK_NUM && startBlock < MAX_BLOCK_NUM;
}

bool Inode::checkDirectoryAttributes() {
    return startBlock == 0 && usedSize == 0;
}

///////////////////////////////////////////////////
// Getters/Setters
///////////////////////////////////////////////////

string Inode::getName() {
    return string(name);
}

void Inode::setName(string newName) {
    strcpy(name, newName.c_str());
}

uint8_t Inode::getUsedSize() {
    return usedSize;
}

void Inode::setUsedSize(uint8_t newSize) {
    usedSize = newSize;
}

uint8_t Inode::getStartBlock() {
    return startBlock;
}

void Inode::setStartBlock(uint8_t newStartBlock) {
    startBlock = newStartBlock;
}

uint8_t Inode::getParent() {
    return parent;
}

void Inode::setParent(uint8_t newParent) {
    parent = newParent;
}