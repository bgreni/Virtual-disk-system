#pragma once

#include <string>
#include <stdint.h>
using namespace std;


class Inode {
	private:
		char name[5];        // Name of the file or directory
		uint8_t usedSize;    // Inode state and the size of the file or directory
		uint8_t startBlock;  // Index of the start file block
		uint8_t parent;  	 // Inode mode and the index of the parent inode
	public:
		Inode();															// default constructor
		Inode(const string &name, int size, int startBlock, int parent);	// constructor with arguments
		string getName();													// returns the name of the file/directory
		void setName(string newName);										// sets the name of the file/directory
		uint8_t getUsedSize();												// returns the number of blocks used by the file
		void setUsedSize(uint8_t newSize);									// sets the number of blocks used by the file
		uint8_t getStartBlock() const;										// returns the index of the first block of the file
		void setStartBlock(uint8_t newStartBlock);							// sets the index of the first block of the file
		uint8_t getParent();												// returns the inode index of the parent directory
		void setParent(uint8_t newParent);									// sets the parent directory
		void setInUse(bool inUse);											// sets if the inode is currently in use
		void setIsFile(bool isFile);										// sets of the inode is assigned to a file
		int getEndIndex();													// returns the index of the last block assigned to the file

		bool nodeInUse();													// returns true if the node is currently in use
		bool blockInNodeRange(int index);									// returns true if index is within the block range of the file
		bool nodeIsClean();													// returns true if all bits of this inode are zero
		bool hasName();														// returns true if the inodes name has a non-zero bit in it
		bool isAFile();														// returns true if the inode is currently attached to a file
		bool checkStartBlock();												// returns true if the inode has a valid start block number
		bool checkDirectoryAttributes();									// checks if the inode is represents a valid directory

		string str(int index);												// returns a string interpretation of the node
};