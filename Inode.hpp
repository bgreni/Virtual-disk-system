#pragma once

#include <string>
#include <stdint.h>
using namespace std;


class Inode {
	private:
		char name[5];        // Name of the file or directory
		uint8_t usedSize;   // Inode state and the size of the file or directory
		uint8_t startBlock; // Index of the start file block
		uint8_t parent;  // Inode mode and the index of the parent inode
	public:
		Inode();
		string getName();
		void setName(string newName);
		uint8_t getUsedSize();
		void setUsedSize(uint8_t newSize);
		uint8_t getStartBlock();
		void setStartBlock(uint8_t newStartBlock);
		uint8_t getParent();
		void setParent(uint8_t newParent);

		bool nodeInUse();
		bool blockInNodeRange(int index);
		bool nodeIsClean();
		bool hasName();
		bool isAFile();
		bool checkStartBlock();
		bool checkDirectoryAttributes();
};