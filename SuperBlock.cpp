#include "SuperBlock.hpp"
#include <iostream>
#include <map>
#include <set>
#include <string>
using namespace std;

SuperBlock::SuperBlock() {}


int SuperBlock::checkConsistency() {
    if (checkFreeList()) {
        return 1;
    }
    // else if (!checkUniqueNames()) {
    //     return 2;
    // }
    return 0;
}



bool SuperBlock::checkFreeList() {

    cout << free_block_list << endl;

    bool blockIsUsed;
    for (int i = 1; i < NUM_BLOCKS; i++) {
        blockIsUsed = false;
        for (int j = 0; j < NUM_NODES; j++) {
            if (inode[j].blockInNodeRange(i)) {
                // block should be free but isn't
                if (free_block_list[i] != 1)
                    return false;
                else {
                    // block is used by more than one node
                    if (blockIsUsed) {
                        return false;
                    } else {
                        blockIsUsed = true;
                    }
                }
            }
        }
        if (free_block_list[i] == 1 && !blockIsUsed) {
            return false;
        }
    }
    return true;
}



bool SuperBlock::checkUniqueNames() {
    map<int, set<string>> nameMap;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        Inode currNode = inode[i];
        if (nameMap.find(currNode.getParent()) != nameMap.end()) {
            nameMap[currNode.getParent()] = set<string>();
        }
        if (nameMap[currNode.getParent()].find(currNode.getName()) != nameMap[currNode.getParent()].end()) {
            return false;
        }
        nameMap[currNode.getParent()].insert(currNode.getName());
    }

    return true;
}

bool SuperBlock::checkFreeNodes() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        Inode currNode = inode[i];
        if (!currNode.nodeInUse()) {
            if (!currNode.nodeIsClean()) {
                return false;
            }
        } else {
            return currNode.hasName();
        }
    }
}


bool SuperBlock::checkFileStart() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
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
    for (int i = 0; i < NUM_BLOCKS; i++) {
        Inode currNode = inode[i];
        if (!currNode.isAFile()) {
            if (!currNode.checkDirectoryAttributes()) {
                return false;
            }
        }
    }
    return true;
}