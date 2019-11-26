#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <map>
#include "CommandParser.hpp"
#include "FileSystem.hpp"

typedef bool (CommandParser::*FunctionPointer)();
// typedef map<string, FunctionPointer> FuncMap;

const int NUM_NODES = 126;
const int NUM_BLOCKS = 128;
const size_t MAX_NAME_LEN = 5;
const size_t TWO_ARG_COMMAND = 3;
const size_t NO_ARG_COMMAND = 1;
const size_t ONE_ARG_COMMAND = 2;
const size_t LEN_CREATE_COMMAND = 3;
const size_t MAX_BUFF_LEN = 1024;
const size_t MIN_BLOCK_NUM = 1;
const size_t MAX_BLOCK_NUM = 127;
const size_t BLOCK_SIZE = 1024;

const string MOUNT = "M";
const string CREATE = "C";
const string DELETE = "D";
const string READ = "R";
const string WRITE = "W";
const string BUFFER = "B";
const string LS = "L";
const string RESIZE = "E";
const string DEFRAG = "O";
const string CD = "Y";

static map<string, FunctionPointer>  commandMap {
    {"M", &CommandParser::checkMount},
    {"C", &CommandParser::validFileOp},
    {"R", &CommandParser::validReadWrite},
    {"W", &CommandParser::validReadWrite},
    {"E", &CommandParser::validFileOp},
    {"B", &CommandParser::validBuffOp},
    {"L", &CommandParser::validNoArgOp},
    {"O", &CommandParser::validNoArgOp},
    {"C", &CommandParser::validOneArgOp},
    {"D", &CommandParser::validOneArgOp},
};