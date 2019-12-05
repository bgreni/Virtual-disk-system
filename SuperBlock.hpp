#pragma once

#include "Constants.hpp"
#include "Inode.hpp"
#include <bitset>
#include <map>
#include <vector>
using namespace std;

class SuperBlock {
    private:
        bool checkFreeList();
        bool checkUniqueNames();
        bool checkFreeNodes();
        bool checkFileStart();
        bool checkDirectories();
        bool checkNodeParent();
        bool isReservedName(const string &name);
        bool nameUniqueInDir(const string &name, const uint8_t cwd);
        void deleteFile(uint8_t index);
    public:
        bitset<NUM_BLOCKS> free_block_list;
        Inode inode[NUM_NODES];
        SuperBlock();
        void setNode(Inode node, int index);
        void setBlock(int start, int end);
        void clearBlock(int start, int end);
        void fixFreeBlockList();
        
        int checkConsistency();
        int findFreeNode();
        bool validNewName(const string &name, const uint8_t cwd);
        int findContigBlock(const int size);
        void deleteNode(const string &name, const uint8_t cwd);
        uint8_t getInodeIndex(const string &name, const uint8_t cwd);
        Inode getNode(uint8_t index);
        void buildDirectoryMap();
        map<uint8_t, vector<uint8_t>> getDirectoryMap();
        bool isFreeBlock(int start, int end);
        int findNewStartBlock(int oldStart);

        void printFBL();
        void printNodes();
};