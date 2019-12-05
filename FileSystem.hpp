#pragma once

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <bitset>
#include <string>
#include <fstream>
#include <queue>
#include <vector>
#include "SuperBlock.hpp"
using namespace std;

class FileSystem {
	private:
		fstream inputFile;											// the file stream for command inputs
		fstream diskFile;											// file stream for the disk
		uint8_t currentDirectory;									// the index of the cwd in the inode array
		bool diskIsMounted;											// if there is a disk mounted
		string currentDiskName;										// the name of the disk that mounted										
		uint8_t buffer[1024];										// the global buffer
		void clearBuffer();											// zero out global buffer
		void shrinkBlock(uint8_t index, Inode &node, int newSize);	// reducde the size of a file
		void growBlock(uint8_t index, Inode &node, int newSize);	// grow the size of a file
		void copyBlocks(Inode oldNode, Inode newNode);				// copy the contents of a file to a new location
		Inode optimizeBlockLocation(Inode node);						// optimize the start block of a file
		void writeSB();												// write super block to disk
	public:
		SuperBlock superBlock;										// the super block of the disk
		FileSystem();												// default constructor
		void fs_mount(const string &new_disk_name);					// mount a new disk
		void fs_create(const string &name, int size);				// create a file or dir
		void fs_delete(const string &name);							// delete a file of dir
		void fs_read(const string &name, int block_num);			// read from a file
		void fs_write(const string &name, int block_num);			// write to a file
		void fs_buff(uint8_t buff[MAX_BUFF_LEN]);					// put something in the global buffer
		void fs_ls(void);											// print directory structure
		void fs_resize(const string &name, int new_size);			// resize a file
		void fs_defrag(void);										// defragment the disk
		void fs_cd(string &name);									// change cwd
		bool openInputFile(const string &filename);					// open the command input file
		queue<string> readCommands();								// read all commands from input file
		void runCommand(vector<string> tokens);						// run a command
		void close();												// close file streams
};