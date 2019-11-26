#pragma once

#include "Inode.hpp"
#include <bitset>
#include "Constants.hpp"
using namespace std;

class SuperBlock {
    private:
        
        bool checkFreeList();
        bool checkUniqueNames();
        bool checkFreeNodes();
        bool checkFileStart();
        bool checkDirectories();
    public:
    bitset<NUM_BLOCKS> free_block_list;
        Inode inode[NUM_NODES];
        SuperBlock();
        int checkConsistency();
};