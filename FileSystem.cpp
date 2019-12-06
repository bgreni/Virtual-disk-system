#include "FileSystem.hpp"
#include <iostream>  
#include <map>
#include <vector>
#include <cstdio>
#include <algorithm>
using namespace std;

/**
 * using this sorter to create a list of inodes sorted by start block for the defrag function
*/
struct defragSorter
{
    inline bool operator() (const Inode& node1, const Inode& node2)
    {
        return (node1.getStartBlock() < node2.getStartBlock());
    }
};

FileSystem::FileSystem() {
    diskIsMounted = false;
    superBlock = SuperBlock();
}


///////////////////////////////////////////////////
// Main File System Commands
///////////////////////////////////////////////////

/**
 * @brief Mounts a disk into the file system
 * @param new_disk_name - the name of the disk to be mounted
*/
void FileSystem::fs_mount(const string &new_disk_name) {

    fstream newDisk;
    SuperBlock newSB = SuperBlock();;

    newDisk.open(new_disk_name, ios::in | ios::out | ios::binary);
    if (!newDisk.is_open()) {
        cerr << "Error: Cannot find disk: " << new_disk_name << endl;
        return;
    }
    // read the first 1024 bytes into the super block
    newDisk.seekg(0);
    newDisk.read(reinterpret_cast<char*>(&newSB), sizeof(SuperBlock));

    superBlock.fixFreeBlockList();
    // check the consitency of the super block
    int consistencyErrCode = superBlock.checkConsistency();

    if (consistencyErrCode != 0) {
        cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: " << consistencyErrCode << ")" << endl;
        return;
    } else {
        diskIsMounted = true;
        currentDiskName = new_disk_name;
        currentDirectory = ROOT_DIR;
        superBlock.buildDirectoryMap();
    }
    diskFile.close();
    diskFile.open(new_disk_name, ios::in | ios::out | ios::binary);
    superBlock = newSB;
}

/**
 * @brief Create a new file or directory on the disk
 * @param name - the name of the new file/dir
 * @param size - if creating a file size is the number of block that the file has reserved
*/
void FileSystem::fs_create(const string &name, int size) {
    // find the index of the first free inode
    int freeIndex = superBlock.findFreeNode();
    if (freeIndex == -1) {
        cerr << "Error: Superblock in disk " << currentDiskName << " is full, cannot create " << name << endl;
        return;
    }
    if (!superBlock.validNewName(name, currentDirectory)) {
        cerr << "Error: File or directory " << name << " already exists" << endl;
        return;
    }
    int startBlock = 0;
    if (size != 0) {
        startBlock = superBlock.findContigBlock(size);
        if (startBlock == -1) {
            cerr << "Error: cannot allocate " << size << " on " <<currentDiskName << endl;
            return;
        }
    }
    Inode newNode = Inode(name, size, startBlock, currentDirectory);
    superBlock.setNode(newNode, freeIndex);
    superBlock.setBlock(startBlock, startBlock + (size - 1));
    // rebuild the directory map
    superBlock.buildDirectoryMap();
    writeSB();
}

/**
 * @brief delete a node or dir from the disk
 * @param name - the name of the node to be deleted
*/
void FileSystem::fs_delete(const string &name) {
    vector<char> zeros(BLOCK_SIZE, 0);
    int index = superBlock.getInodeIndex(name, currentDirectory);
    Inode node = superBlock.getNode(index);
    int pos = node.getStartBlock() * BLOCK_SIZE;
    // zero out the data blocks
    for (int i = 0; i < node.getUsedSize(); i++) {
        diskFile.seekg(pos);
        diskFile.write(reinterpret_cast<char*>(&zeros), BLOCK_SIZE);
        pos += BLOCK_SIZE;
    }
    superBlock.deleteNode(name, currentDirectory);
    superBlock.buildDirectoryMap();
    writeSB();
}

/**
 * @brief read the block_num'th block of the given file into the global buffer
 * @param name - the name of the file to read from
 * @param block_num - the index of the block to read from w.r.t the first block of the file
*/
void FileSystem::fs_read(const string &name, int block_num) {
    uint8_t index = superBlock.getInodeIndex(name, currentDirectory);
    Inode node = superBlock.getNode(index);
    if (index == INVALID_NODE_NUM || !node.isAFile()) {
        cerr << "Error: " << name << " does not exist" << endl;
        return;
    }
    int size = node.getUsedSize();
    if (block_num < 0 || block_num > size - 1) {
        cerr << name << " does not have block " << block_num << endl;
        return;
    }
    int start = node.getStartBlock();
    int blockToRead = (start+block_num) * BLOCK_SIZE;
    diskFile.seekg(blockToRead);
    diskFile.read(reinterpret_cast<char*>(buffer), MAX_BUFF_LEN);
}

/**
 * @brief writes the contents of the global buffer to the block_num'th block of the given file
 * @param name - the name of the file to write to
 * @param block_num - the index of the block to write to w.r.t to the first block of the file
*/
void FileSystem::fs_write(const string &name, int block_num) {
    uint8_t index = superBlock.getInodeIndex(name, currentDirectory);
    Inode node = superBlock.getNode(index);
    if (index == INVALID_NODE_NUM || !node.isAFile()) {
        cerr << "Error: " << name << " does not exist" << endl;
        return;
    }
    int size = node.getUsedSize();
    if (block_num < 0 || block_num > size - 1) {
        cerr << name << " does not have block " << block_num << endl;
        return;
    }
    int start = node.getStartBlock();
    int blockToRead = (start+block_num) * BLOCK_SIZE;
    diskFile.seekg(blockToRead);
    diskFile.write(reinterpret_cast<char*>(buffer), MAX_BUFF_LEN);
    superBlock.buildDirectoryMap();
    writeSB();
}

/**
 * @brief copies the contents of the passed buffer to the global buffer
 * @param buff - an int array holding the data to be written to the global buffer
*/
void FileSystem::fs_buff(uint8_t buff[MAX_BUFF_LEN]) {
    // clear the buffer to ensure no garbage values exist
    clearBuffer();
    for (size_t i = 0; i < MAX_BUFF_LEN; i++) {
        buffer[i] = buff[i];
    }
}


// just sneaking these in here

void printDir(const char* name, const int numChildren) {
    // using printf so its easier to get the formating right
    printf("%-5s %3d\n",name, numChildren);
}

void printFile(const char* name, const int size) {
    printf("%-5s %3d KB\n", name, size);
}

/**
 * @brief prints the contents of the current working directory
*/
void FileSystem::fs_ls(void) {
    cout << endl;
    superBlock.buildDirectoryMap();
    // get the directory hierarchy of the disk
    map<uint8_t, vector<uint8_t>> directoryStructure = superBlock.getDirectoryMap();
    vector<uint8_t> dirContents = directoryStructure[currentDirectory];
    // size is always +2 due to "." and ".."
    int size = dirContents.size() + 2;
    // print cwd
    printDir(CUR_DIR_STRING.c_str(), size);
    if (currentDirectory == ROOT_DIR) {
        // if in root directory "." == ".."
        printDir(PARENT_DIR_STRING.c_str(), size);
    } else {
        // print parent directory if not in root
        Inode node = superBlock.getNode(currentDirectory);
        uint8_t parent = node.getParent();
        vector<uint8_t> parentContents = directoryStructure[parent];
        int parentSize = parentContents.size() + 2;
        printDir(PARENT_DIR_STRING.c_str(), parentSize);
    }

    // print all files/dirs within the current working directory
    for (auto index : dirContents) {
        Inode node = superBlock.getNode(index);
        if (node.isAFile()) {
            printFile(node.getName().c_str(), node.getUsedSize());
        } else {
            vector<uint8_t> childContents = directoryStructure[index];
            int childSize = childContents.size() + 2;
            printDir(node.getName().c_str(), childSize);
        }
    }
}

/**
 * @brief resizes a file in on the disk
 * @param name - the name of the file to be resized
 * @param new_size - the new size of the file, can be smaller or bigger than original size
*/
void FileSystem::fs_resize(const string &name, int new_size) {
    uint8_t index = superBlock.getInodeIndex(name, currentDirectory);
    Inode node = superBlock.getNode(index);
    if ((size_t)new_size > MAX_BLOCK_NUM) {
        cerr << "Error: File " << node.getName() << " cannot be expanded to size " << new_size << endl;
        return;
    }
    if (new_size == node.getUsedSize()) return;
    if (index == INVALID_NODE_NUM || !node.isAFile()) {
        cerr << "Error: " << name << " does not exist" << endl;
        return;
    }

    if (new_size == node.getUsedSize()) {
        return;
    }

    if (new_size < node.getUsedSize()) {
        shrinkBlock(index, node, new_size);
    } else {
        growBlock(index, node, new_size);
    }
    writeSB();
}

/**
 * @brief shrinks the file of the given node 
 * alter the inode to reflect the changes, clear the required section of the
 * free block list, and zero out the now unused parts of the file
 * @param index - the index of the inode for this file
 * @param node - the node to be altered
 * @param newSize - the new size of the file
*/
void FileSystem::shrinkBlock(uint8_t index, Inode &node, int newSize) {
    int oldEnd = node.getEndIndex();
    node.setUsedSize(newSize);
    int newEnd = node.getEndIndex();
    superBlock.clearBlock(newEnd + 1, oldEnd);

    uint8_t buf[BLOCK_SIZE];
    // making sure everything is zeroed out
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        buf[i] = 0;
    }

    // zero out unused blocks
    int start = oldEnd * BLOCK_SIZE;
    for (int i = oldEnd; i >= newEnd + 1; i--) {
        diskFile.seekg(start);
        diskFile.write(reinterpret_cast<char*>(buf), BLOCK_SIZE);
    }
    superBlock.setNode(node, index);
}


/**
 * @brief increase the number of blocks allocated to given file
 * if not possible to grow the block from its current start, find a new 
 * contiguous block to put the file
 * @param index - the index of the inode for this file
 * @param node - the node to be altered
 * @param newSize - the new size of the file
*/
void FileSystem::growBlock(uint8_t index, Inode &node, int newSize) {
    int oldEnd = node.getEndIndex();
    node.setUsedSize(newSize);
    int newEnd = node.getEndIndex();
    if (superBlock.isFreeBlock(oldEnd + 1, newEnd)) {
        superBlock.setBlock(oldEnd + 1, newEnd);
    } else {
        int newStart = superBlock.findContigBlock(newSize);
        if (newStart == -1) {
            cerr << "Error: File " << node.getName() << " cannot be expanded to size " << newSize << endl;
            return;
        }
        Inode newNode = node;
        superBlock.clearBlock(node.getStartBlock(), oldEnd);
        newNode.setStartBlock(newStart);
        superBlock.setBlock(newStart, newNode.getEndIndex());
        copyBlocks(node, newNode);
        superBlock.setNode(newNode, index);
    }
}

/**
 * @brief copies the contents of a file from one set of blocks to another
 * @param oldNode - the node for the old file posistion
 * @param newNode - the node for the new file posistion
*/
void FileSystem::copyBlocks(Inode oldNode, Inode newNode) {
    // use a new buffer in case the global buffer still has something we need
    uint8_t buf[BLOCK_SIZE];
    int oldNodePos = oldNode.getStartBlock() * BLOCK_SIZE;
    int newNodePos = newNode.getStartBlock() * BLOCK_SIZE;
    for (int i = 0; i < oldNode.getUsedSize(); i++) {
        diskFile.seekg(oldNodePos);
        diskFile.read(reinterpret_cast<char*>(buf), BLOCK_SIZE);
        diskFile.seekg(newNodePos);
        diskFile.write(reinterpret_cast<char*>(buf), BLOCK_SIZE);

        oldNodePos += BLOCK_SIZE;
        newNodePos += BLOCK_SIZE;
    }
}

/**
 * @brief defragment the disk to optimize space usage
*/
void FileSystem::fs_defrag(void) {
    vector<Inode> nodeList;
    nodeList.reserve(127);
    for (size_t i = 0; i < MAX_BLOCK_NUM; i++) {
        Inode node = superBlock.getNode(i);
        // get all active files in the system
        if (node.nodeInUse() && node.isAFile()) {
            nodeList.push_back(node);
        }
    }
    // sort the node by start block for easier defragmentation
    sort(nodeList.begin(), nodeList.end(), defragSorter());
    for (auto n : nodeList) {
        // optimize the start location of each block
        Inode newNode = optimizeBlockLocation(n);
        if (newNode.nodeInUse()) {
            int index = superBlock.getInodeIndex(newNode.getName(), newNode.getParent());
            superBlock.setNode(newNode, index);
        }
    }
    writeSB();
}

/**
 * @brief move the given node to begin at the first open block
 * @param node - the node of the file to be moved
*/
Inode FileSystem::optimizeBlockLocation(Inode node) {
    int index = superBlock.findNewStartBlock(node.getStartBlock());
    if (index == -1) {
        // block is already at the optimal position
        return Inode();
    }
    Inode newNode = node;
    // cout << superBlock.free_block_list << endl;
    // cout << index << endl;

    // modify the free block list as required
    superBlock.clearBlock(node.getStartBlock(), node.getEndIndex());
    newNode.setStartBlock(index);
    superBlock.setBlock(newNode.getStartBlock(), newNode.getEndIndex());

    uint8_t zeroBuf[BLOCK_SIZE];
    uint8_t readBuf[BLOCK_SIZE];
    // making sure everything is zeroed out
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        zeroBuf[i] = 0;
    }

    int oldStart = node.getStartBlock();

    int pos = oldStart * BLOCK_SIZE;
    int newPos = newNode.getStartBlock() * BLOCK_SIZE;
    // copy file data from old block to new block
    for (int i = oldStart; i < node.getEndIndex(); i++) {
        diskFile.seekg(pos);
        diskFile.read(reinterpret_cast<char*>(readBuf), BLOCK_SIZE);
        diskFile.write(reinterpret_cast<char*>(zeroBuf), BLOCK_SIZE);

        diskFile.seekg(newPos);
        diskFile.write(reinterpret_cast<char*>(readBuf), BLOCK_SIZE);

        pos += BLOCK_SIZE;
        newPos += BLOCK_SIZE;
    }
    return newNode;
}

/**
 * @brief change the current working directory of the file system
 * @param name - the name of the directory to swtich to
*/
void FileSystem::fs_cd(string &name) {
    // if name is "." do nothing
    if (name == CUR_DIR_STRING) {
        return;
    }
    // if name is ".." got to parent
    else if (name == PARENT_DIR_STRING) {
        if (currentDirectory == ROOT_DIR) {
            return;
        }
        currentDirectory = superBlock.getNode(currentDirectory).getParent();
    } else {
        // change to child dir
        int index = superBlock.getInodeIndex(name, currentDirectory);
        if (index == INVALID_NODE_NUM) {
            cerr << "Error: Directory " << name << " does not exist" << endl;
            return; 
        }
        currentDirectory = index;
    }
}


///////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////

/**
 * @brief open the file that contains the list of commands to be executed
 * @param filename - the name of the input file
 * @return bool - if opening the file was successful
*/
bool FileSystem::openInputFile(const string &filename) {
    inputFile.open(filename, ios::in);
    if (!inputFile.is_open()) {
        cerr << "Error: input file does not exist: "  << filename << endl;
        return false;
    }
    return true;
}

/**
 * @brief read the commands from the input file into a queue for execution
 * @return queue<string> - a queue of command strings
*/
queue<string> FileSystem::readCommands() {
    queue<string> commandQueue;
    string s;
    while (getline(inputFile, s)) {
        commandQueue.push(s);
    }
    return commandQueue;
}

/**
 * @brief parse and run a command
 * @param tokens - a tokenized version of the command string
*/
void FileSystem::runCommand(vector<string> tokens) {

    string command = tokens[0];

    if (!diskIsMounted && command != MOUNT) {
        cerr << "Error: No file system is mounted" << endl;
        return;
    }
    
    // cant do a switch on a string I guess so here we are

    if (command == MOUNT) {
        fs_mount(tokens[1]);
    }
    else if (command == CREATE) {
        int size = stoi(tokens[2]);
        fs_create(tokens[1], size);
    }
    else if (command == DELETE) {
        fs_delete(tokens[1]);
    }
    else if (command == READ) {
        int blockNum = stoi(tokens[2]);
        fs_read(tokens[1], blockNum);
    }
    else if (command == WRITE) {
        int blockNum = stoi(tokens[2]);
        fs_write(tokens[1], blockNum);
    }
    else if (command == BUFFER) {
        fs_buff((uint8_t*)tokens[1].c_str());
    }
    else if (command == LS) {
        fs_ls();
    }
    else if (command == RESIZE) {
        int newSize = stoi(tokens[2]);
        fs_resize(tokens[1], newSize);
    }
    else if (command == DEFRAG) {
        fs_defrag();
    }
    else if (command == CD) {
        fs_cd(tokens[1]);
    }
}

/**
 * @brief close the two file streams that we use
*/
void FileSystem::close() {
    diskFile.close();
    inputFile.close();
}


/**
 * @brief zero out the global buffer
*/
void FileSystem::clearBuffer() {
    for (size_t i = 0; i < MAX_BUFF_LEN; i++) {
        buffer[i] = 0;
    }
}


/**
 * @brief write the super block back to the file
*/
void FileSystem::writeSB() {
    /**
     * I was already 1200 lines deep into this project when I realized that reordering the free block list would ruin
     * everything when I write it back to the disk. I realize switching it back the original ordering and back again 
     * everytime I write back to the disk is awful please have mercy on my soul
    */
    superBlock.fixFreeBlockList();
    diskFile.seekg(0);
    diskFile.write(reinterpret_cast<char*>(&superBlock), sizeof(SuperBlock));
    superBlock.fixFreeBlockList();
}








