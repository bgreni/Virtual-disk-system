#include "Inode.hpp"

using namespace std;

Inode::Inode() {}

bool Inode::nodeInUse() {
    return (usedSize >> 7) == 1;
}

bool Inode::blockInNodeRange(int index) {
    return index >= startBlock && index <= startBlock + usedSize - 1;
}

///////////////////////////////////////////////////
// Getters/Setters
///////////////////////////////////////////////////

string Inode::getName() {
    return name;
}

void Inode::setName(string newName) {
    name = newName;
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