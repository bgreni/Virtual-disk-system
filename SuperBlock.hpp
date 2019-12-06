#pragma once

#include "Constants.hpp"
#include "Inode.hpp"
#include <bitset>
#include <map>
#include <vector>
using namespace std;

class SuperBlock {
    private:
        bool checkFreeList();                                           // check that the free block list is consistent with inodes
        bool checkUniqueNames();                                        // checks that each directory contains elements with unique names
        bool checkFreeNodes();                                          // checks that all free nodes are also clean
        bool checkFileStart();                                          // checks that all files hav valid start positions
        bool checkDirectories();                                        // checks that all dirs hav valid attributes
        bool checkNodeParent();                                         // checks that all nodes have valid parents
        bool isReservedName(const string &name);                        // checks that a given name isn't on of the reserved names
        bool nameUniqueInDir(const string &name, const uint8_t cwd);    // checks that a name is unique in a given directory
        void deleteFile(uint8_t index);                                 // delete a file in the superblock
        bitset<NUM_BLOCKS> free_block_list;                             // bitset representing free blocks in the file
        Inode inode[NUM_NODES];                                         // an array of all the inodes
    public:
        SuperBlock();                                                   // default constructor
        void setNode(Inode node, int index);                
        void setBlock(int start, int end);
        void clearBlock(int start, int end);
        void fixFreeBlockList();                                        // flips the ordering of every 8 bits in the bitset
        
        int checkConsistency();                                         // runs consistency check on the superblock
        int findFreeNode();                                             // returns the index of the first free node
        bool validNewName(const string &name, const uint8_t cwd);       // checks that a given name is valid in the given directory
        int findContigBlock(const int size);                            // returns the index of the first section of blocks that can hold "size" number blocks of data
        void deleteNode(const string &name, const uint8_t cwd);         // delete a node
        uint8_t getInodeIndex(const string &name, const uint8_t cwd);   // get the index of a node in the inode array given its name
        Inode getNode(uint8_t index);                                   // get a node given its index
        void buildDirectoryMap();                                       // build a map representation of the directory structure
        map<uint8_t, vector<uint8_t>> getDirectoryMap();                
        bool isFreeBlock(int start, int end);                           // checks if a section of blocks are all free
        int findNewStartBlock(int oldStart);                            // returns the index to a new start block for a file

        void printFBL();
        void printNodes();
};