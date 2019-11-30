#include "SuperBlock.hpp"
#include <iostream>
#include <string>
using namespace std;

map<uint8_t, vector<uint8_t>> directoryStructure;

SuperBlock::SuperBlock() {
    for (int i = 0; i < NUM_NODES; i++) {
        inode[i] = Inode();
    }
}


void SuperBlock::setNode(Inode node, int index) {
    inode[index] = node;
}


Inode SuperBlock::getNode(uint8_t index) {
    if (index == INVALID_NODE_NUM) {
        return Inode();
    }
    return inode[index];
}

void SuperBlock::setBlock(int start, int end) {
    for (int i = start; i <= end; i++) {
        if (free_block_list[i] == 1) {
            cerr << "Brian you fucked up" << endl;
            return;
        }
        free_block_list[i] = 1;
    }
}


void SuperBlock::clearBlock(int start, int end) { 
    for (int i = start; i <= end; i++) {
        if (free_block_list[i] == 0) {
            cerr << "Brian you fucked up" << endl;
            return;
        }
        free_block_list[i] = 0;
    }
}

map<uint8_t, vector<uint8_t>> SuperBlock::getDirectoryMap() {
    return directoryStructure;
}

// TODO FIX NAME IN DIRECTORY CHECK

///////////////////////////////////////////////////
// Consistency Checks
///////////////////////////////////////////////////


int SuperBlock::checkConsistency() {
    if (!checkFreeList()) {
        return 1;
    }
    else if (!checkUniqueNames()) {
        return 2;
    }
    else if (!checkFreeNodes()) {
        return 3;
    }
    else if (!checkFileStart()) {
        return 4;
    }
    else if (!checkDirectories()) {
        return 5;
    }
    else if (!checkNodeParent()) {
        return 6;
    }
    return 0;
}



bool SuperBlock::checkFreeList() {

    // cout << free_block_list << endl;

    bool blockIsUsed;
    for (int i = 1; i < NUM_BLOCKS; i++) {
        blockIsUsed = false;
        for (int j = 0; j < NUM_NODES; j++) {
            if (inode[j].blockInNodeRange(i)) {
                // block should be free but isn't
                if (free_block_list[i] != 1) {
                    cout << "block " << i << " should be free but isn't, in use by inode: " << j << endl;
                    return false;
                } else {
                    // block is used by more than one node
                    if (blockIsUsed) {
                        cout << "block " << i << " is used by more than one node" << endl;
                        return false;
                    } else {
                        // cout << "block: " << i << " is in use" << endl;
                        blockIsUsed = true;
                    }
                }
            }
        }
        if (free_block_list[i] == 1 && !blockIsUsed) {
            cout << "block " << i << " should be in use but isn't" << endl;
            return false;
        }
    }
    return true;
}



bool SuperBlock::checkUniqueNames() {
    map<int, set<string>> nameMap;
    for (int i = 0; i < NUM_NODES; i++) {
        Inode currNode = inode[i];
        if (currNode.hasName()) {
            // init index if doesn't exist 
            if (nameMap.find(currNode.getParent()) == nameMap.end()) {
                nameMap[currNode.getParent()] = set<string>();
            }
            if (nameMap[currNode.getParent()].find(currNode.getName()) != nameMap[currNode.getParent()].end()) {
                return false;
            }
            nameMap[currNode.getParent()].insert(currNode.getName());
        }
    }

    return true;
}

bool SuperBlock::checkFreeNodes() {
    for (int i = 0; i < NUM_NODES; i++) {
        Inode currNode = inode[i];
        if (!currNode.nodeInUse()) {
            // node isn't in use but has non-zero bits
            if (!currNode.nodeIsClean()) {
                return false;
            }
        } else {
            // node is in use but no name
            if (!currNode.hasName()) {
                return false;
            }
        }
    }
    return true;
}


bool SuperBlock::checkFileStart() {
    for (int i = 0; i < NUM_NODES; i++) {
        Inode currNode = inode[i];
        if (currNode.isAFile()) {
            if (!currNode.checkStartBlock()) {
                return false;
            }
        }
    }
    return true;
}



bool SuperBlock::checkDirectories() {
    for (int i = 0; i < NUM_NODES; i++) {
        Inode currNode = inode[i];
        if (!currNode.isAFile()) {
            if (!currNode.checkDirectoryAttributes()) {
                return false;
            }
        }
    }
    return true;
}

bool SuperBlock::checkNodeParent() {
    for (int i = 0; i < NUM_NODES; i++) {
        Inode currNode = inode[i];
        uint8_t parent = currNode.getParent();
        if (currNode.nodeInUse()) {
            // invalid node parent
            if (parent == 126) {
                return false;
            }
            else if (parent >= 0 && parent <= 125) {
                if (!inode[parent].nodeInUse() || !inode[parent].isAFile()) {
                    return false;
                }
            }
        }
    }
    return true;

}


///////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////

// this is how bad I didn't want to fiddle with bits
void SuperBlock::fixFreeBlockList() {
    int k;
    for (int i = 0; i < (NUM_BLOCKS / BITS_IN_BYTE); i++) {
        for (int j = (i * BITS_IN_BYTE); j < (BITS_IN_BYTE / 2); j++) {
            k = BITS_IN_BYTE - j - 1;
            swap(free_block_list[j], free_block_list[k]);
        }
    }
}

int SuperBlock::findFreeNode() {
    for (int i = 0; i < NUM_NODES; i++) {
        if (!inode[i].nodeInUse()) {
            return i;
        }
    }
    return -1;
}

void SuperBlock::buildDirectoryMap() {
    directoryStructure.clear();
    for (int i = 0; i < NUM_NODES; i++) {
        uint8_t parent = inode[i].getParent();
        if (directoryStructure.find(parent) == directoryStructure.end()) {
            directoryStructure[parent] = vector<uint8_t>();
        }
        directoryStructure[parent].push_back(i);
    }
}

bool SuperBlock::isReservedName(const string &name) {
    return name.compare(".") == 0 || name.compare("..") == 0;
}


bool SuperBlock::nameUniqueInDir(const string &name, const uint8_t cwd) {
    for (auto index : directoryStructure[cwd]) {
        if (name.compare(inode[index].getName()) == 0) {
            return false;
        }
    }
    return true;
}


bool SuperBlock::validNewName(const string &name, const uint8_t cwd) {
    if (isReservedName(name)) {
        return false;
    }
    return true;
    return nameUniqueInDir(name, cwd);
}

int SuperBlock::findContigBlock(const int size) {
    // check if theres enough free spaces at all
    // if (size > (free_block_list.size() - free_block_list.count()))
    size_t i = 1;
    while (i < NUM_BLOCKS) {
        if (free_block_list[i] == 0) {
            for (size_t j = i; j < i+size; j++) {
                if (free_block_list[j] == 1) {
                    i = j;
                    break;
                }
                if (j == (i+size) -1) {
                    return i;
                }
            }
        }
        i++;
    }
    return -1;
}


 uint8_t SuperBlock::getInodeIndex(const string &name, const uint8_t cwd) {
    vector<uint8_t> inCWD = directoryStructure[cwd];
    for (auto item : inCWD) {
        Inode node = inode[item];
        if (name == node.getName()) {
            return item;
        }
    }
    return INVALID_NODE_NUM;
}


void SuperBlock::deleteNode(const string &name, const uint8_t cwd) {
    uint8_t index = getInodeIndex(name, cwd);
    if (index == INVALID_NODE_NUM) {
        cerr << "Error: File or directory " << name << " does not exist" << endl;
        return;
    }
    if (inode[index].isAFile()) {
        deleteFile(index);
    } else {
        for (auto child : directoryStructure[index]) {
            Inode childNode = inode[child];
            deleteNode(childNode.getName(), index);
        }
    }
}


void SuperBlock::deleteFile(uint8_t index) {
    Inode n = inode[index];
    int start = n.getStartBlock();
    int end = n.getEndIndex();
    clearBlock(start, end);
    inode[index] = Inode();
}


bool SuperBlock::isFreeBlock(int start, int end) {
    for (int i = start+1; i <= end; i++) {
        if (free_block_list[i] == 1) 
            return false;
    }
    return true;
}