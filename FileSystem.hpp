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
using namespace std;

class FileSystem {
	private:
		fstream inputFile;
		fstream diskFile;
		string currentDirectory;
		bool diskIsMounted;
	public:
		FileSystem();
		void fs_mount(const string &new_disk_name);
		void fs_create(const string &name, int size);
		void fs_delete(const string &name);
		void fs_read(const string &name, int block_num);
		void fs_write(const string &name, int block_num);
		void fs_buff(u_int8_t buff[1024]);
		void fs_ls(void);
		void fs_resize(const string &name, int new_size);
		void fs_defrag(void);
		void fs_cd(string &name);
		bool openInputFile(const string &filename);
		queue<string> readCommands();
		void runCommand(vector<string> tokens);
};