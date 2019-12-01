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
		fstream inputFile;
		fstream diskFile;
		uint8_t currentDirectory;
		bool diskIsMounted;
		string currentDiskName;
		uint8_t buffer[1024];
		void clearBuffer();
		void shrinkBlock(uint8_t index, Inode &node, int newSize);
		void growBlock(uint8_t index, Inode &node, int newSize);
		void copyBlocks(Inode oldNode, Inode newNode);
		void optimizeBlockLocation(Inode node);
	public:
		SuperBlock superBlock;
		FileSystem();
		void fs_mount(const string &new_disk_name);
		void fs_create(const string &name, int size);
		void fs_delete(const string &name);
		void fs_read(const string &name, int block_num);
		void fs_write(const string &name, int block_num);
		void fs_buff(uint8_t buff[MAX_BUFF_LEN]);
		void fs_ls(void);
		void fs_resize(const string &name, int new_size);
		void fs_defrag(void);
		void fs_cd(string &name);
		bool openInputFile(const string &filename);
		queue<string> readCommands();
		void runCommand(vector<string> tokens);
		void close();
};