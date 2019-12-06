#include "Inode.hpp"
#include "Constants.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;

/**
 * @brief default constructor
*/
Inode::Inode() {
    usedSize = 0;
    startBlock = 0;
    parent = 0;
    for (int i = 0; i < 5; i++) {
        name[i] = 0;
    }
}

/**
 * @brief constructor with arguments
 * @param name - the name of the file/directory
 * @param size - the size of the file, is 0 if its a directory
 * @param startBlock - the index of the first block used by the file, also 0 if a directory
 * @param parent - the index of the parent node of this node
*/
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

/**
 * @brief returns true if the node is currently in use, denoted by the first bit of the usedSize
 * @return bool - whether the first bit of usedSize if 1
*/
bool Inode::nodeInUse() {
    return (usedSize >> 7) == 1;
}

/**
 * @brief returns true if the index is within the range of the blocks used by the node
 * @param index - the block number to check
 * @return - whether the index is in the range of the node
*/
bool Inode::blockInNodeRange(int index) {
    return index >= startBlock && index <= getEndIndex();
}

/**
 * @brief returns the block index of the last block used by the node
 * @return int - the index of the block
*/
int Inode::getEndIndex() {
    return (startBlock + getUsedSize()) - 1;
}

/**
 * @brief returns true if every bit in this node is zero
 * @return bool - true if the node is clean
*/
bool Inode::nodeIsClean() {
    return startBlock == 0 && usedSize == 0 && parent == 0 && !hasName();
}

/**
 * @brief returns true if the name of the node has a non-zero bit in it
 * @return bool - if the name has a non-zero bit
*/
bool Inode::hasName() {
    bool hasChar = false;
    for (size_t i = 0; i < MAX_NAME_LEN; i++) {
        if (name[i] != 0) {
            hasChar = true;
        }
    }
    return hasChar;
}

/**
 * @brief returns true if the node is currently representing a file
 * @return bool - if this node represents a file
*/
bool Inode::isAFile() {
    return ((parent >> 7) == 0) && nodeInUse();
}

/**
 * @brief returns true if this node has a valid start block
 * @return bool
*/
bool Inode::checkStartBlock() {
    return startBlock >= MIN_BLOCK_NUM && startBlock <= MAX_BLOCK_NUM;
}

/**
 * @brief checks if this node represents a valid directory
 * @return bool
*/
bool Inode::checkDirectoryAttributes() {
    return startBlock == 0 && getUsedSize() == 0;
}

///////////////////////////////////////////////////
// Getters/Setters
///////////////////////////////////////////////////

string Inode::getName() {
    /**
     * converting from char array to string
     * since the char array might not have a null byte, converting to
     * a string via constructor was a bit buggy, so using a stringstream 
    */
    stringstream ss;
    for(size_t i = 0; i < MAX_NAME_LEN; i++) {
        if (name[i] == 0) {
            break;
        }
        ss << name[i];
    }
    return ss.str();
}

void Inode::setName(string newName) {
    size_t len = newName.length();
    newName.copy(name, len);
    if (len < MAX_NAME_LEN) {
        name[len] = '\0';
    }
}

uint8_t Inode::getUsedSize() {
    return (usedSize & ALL_BUT_LAST_MASK);
}

void Inode::setUsedSize(uint8_t newSize) {
    int stateBit = usedSize & LAST_BIT_MASK;
    usedSize = newSize;
    usedSize |= stateBit;
}

uint8_t Inode::getStartBlock() const {
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


string Inode::str(int index) {
    stringstream ss;
    ss << "-----------------------------" << endl;
    ss << "Index: " << index << endl;
    ss << "Used Size: " << (int)getUsedSize() << endl;
    ss << "Start Block: " << (int)getStartBlock() << endl;
    ss << "Name: " << getName() << endl;
    ss << "Is a File: " << isAFile() << endl;
    ss << "Is in use: " << nodeInUse() << endl;
    ss << "Parent: " << (int)getParent() << endl;
    ss << "-----------------------------" << endl;
    return ss.str();
}