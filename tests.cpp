#include "FileSystem.hpp"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

stringstream err;
stringstream out;

streambuf *olderr;
streambuf *oldout;


void setup();
void resetIO();
bool testMount();
bool testBadDiskName();
bool testFreeListCheck();
bool testUniqueNames();
bool testCheckInodes();
bool testFileStart();
bool testDirecAtt();
bool testCheckParents();

int main() {
    setup();
    if (!testMount()) return 1;
    err.flush();
    resetIO();
    cout << "passed all tests!" << endl;
    return 0;
}

void setup() {
    oldout = cout.rdbuf(out.rdbuf());
    olderr = cerr.rdbuf(err.rdbuf());
}

void resetIO() {
    cout.rdbuf(oldout);
    cerr.rdbuf(olderr);
}

///////////////////////////////////////////////////
// Mount Tests
///////////////////////////////////////////////////
string validName = "disk1";

bool testMount() {
    if (!testBadDiskName()) {
        resetIO();
        cout << "Failed disk name test" << endl;
        return false;
    }
    if (!testFreeListCheck()) {
        resetIO();
        cout << "Failed free list test" << endl;
        return false;
    }
    if (!testUniqueNames()) {
        resetIO();
        cout << "Failed unique names test" << endl;
        return false;
    }
    if (!testCheckInodes()) {
        resetIO();
        cout << "Failed Inodes test" << endl;
        return false;
    }
    if (!testFileStart()) {
        resetIO();
        cout << "Failed file start test" << endl;
        return false;
    }
    if (!testDirecAtt()) {
        resetIO();
        cout << "Failed directory attribute test" << endl;
        return false;
    }
    if (!testCheckParents()) {
        resetIO();
        cout << "Failed parent check test" << endl;
        return false;
    }
    resetIO();
    return true;
}

bool testBadDiskName() {
    FileSystem fs = FileSystem();
    string name = "dned";
    fs.fs_mount(name);
    string error = err.str();
    string expected = "Error: Cannot find disk: " + name + "\n";
    return error == expected;
}

bool testFreeListCheck() {
    FileSystem fs = FileSystem();
    fs.superBlock.inode[0].setStartBlock(3);
    fs.superBlock.inode[0].setUsedSize(1);
    int error = fs.superBlock.checkConsistency();
    int expected = 1;
    if (error != expected) return false;
    fs = FileSystem();
    fs.superBlock.free_block_list[4] = 1;
    error = fs.superBlock.checkConsistency();
    return error == expected;
}

bool testUniqueNames() {
    FileSystem fs = FileSystem();
    fs.superBlock.inode[0].setParent(127);
    fs.superBlock.inode[0].setName("c");
    fs.superBlock.inode[1].setParent(127);
    fs.superBlock.inode[1].setName("c");
    int error = fs.superBlock.checkConsistency();
    int expected = 2;
    return error == expected;
}

bool testCheckInodes() {
    FileSystem fs = FileSystem();
    fs.superBlock.inode[0].setStartBlock(3);
    int error = fs.superBlock.checkConsistency();
    int expected = 3;
    if (error != expected) return false;
    fs = FileSystem();
    fs.superBlock.free_block_list[1] = 1;
    fs.superBlock.inode[0].setInUse(true);
    fs.superBlock.inode[0].setStartBlock(1);
    fs.superBlock.inode[0].setUsedSize(1);
    error = fs.superBlock.checkConsistency();
    return error == expected;
}

bool testFileStart() {
    FileSystem fs = FileSystem();
    fs.superBlock.inode[0].setIsFile(true);
    fs.superBlock.inode[0].setInUse(true);
    fs.superBlock.inode[0].setName("c");
    fs.superBlock.inode[0].setStartBlock(129);
    int error = fs.superBlock.checkConsistency();
    int expected = 4;
    return error == expected;
}

bool testDirecAtt() {
    FileSystem fs = FileSystem();
    fs.superBlock.inode[0].setIsFile(false);
    fs.superBlock.inode[0].setInUse(true);
    fs.superBlock.inode[0].setName("c");
    fs.superBlock.inode[0].setUsedSize(1);
    int error = fs.superBlock.checkConsistency();
    int expected = 5;
    return error == expected;
}

bool testCheckParents() {
    resetIO();
    FileSystem fs = FileSystem();
    fs.superBlock.inode[0].setIsFile(false);
    fs.superBlock.inode[0].setInUse(true);
    fs.superBlock.inode[0].setName("c");
    fs.superBlock.inode[0].setParent(126);
    int error = fs.superBlock.checkConsistency();
    int expected = 6;
    if (error != expected) return false;
    fs.superBlock.inode[0].setParent(1);
    fs.superBlock.inode[1].setInUse(true);
    fs.superBlock.inode[1].setIsFile(true);
    fs.superBlock.inode[1].setName("b");
    fs.superBlock.inode[1].setStartBlock(1);
    fs.superBlock.inode[1].setUsedSize(1);
    fs.superBlock.free_block_list[1] = 1;
    error = fs.superBlock.checkConsistency();
    return error == expected;
}


