#include "SuperBlock.hpp"
#include <iostream>
using namespace std;

SuperBlock::SuperBlock() {}


int SuperBlock::checkConsistency() {
    if (!checkFreeList()) {
        return 1;
    }
    return 0;
}



bool SuperBlock::checkFreeList() {

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