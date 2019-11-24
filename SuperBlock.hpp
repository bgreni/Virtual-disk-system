#pragma once

#include "Inode.hpp"
#include <bitset>
#include "Constants.hpp"
using namespace std;

class SuperBlock {
    private:
        bitset<NUM_BLOCKS> free_block_list;
        Inode inode[NUM_NODES];
        bool checkFreeList();
        bool checkUniqueNames();
    public:
        SuperBlock();
        int checkConsistency();
};