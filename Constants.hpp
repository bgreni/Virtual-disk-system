#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <map>
#include "CommandParser.hpp"

typedef bool (CommandParser::*FunctionPointer)();

/**
 * Putting all my constants here for the good practice
*/


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
const size_t BITS_IN_BYTE = 8;

const uint8_t ROOT_DIR = 127;
const uint8_t INVALID_NODE_NUM = 129;

const size_t LAST_BIT_MASK = 0x80;
const size_t ALL_BUT_LAST_MASK = 0x7F;
const size_t NO_BITS_MASK = 0;

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

const string CUR_DIR_STRING = ".";
const string PARENT_DIR_STRING = "..";

static map<string, FunctionPointer>  commandMap {
    {"M", &CommandParser::checkMount},
    {"C", &CommandParser::validCreateOp},
    {"R", &CommandParser::validReadWrite},
    {"W", &CommandParser::validReadWrite},
    {"E", &CommandParser::validFileOp},
    {"B", &CommandParser::validBuffOp},
    {"L", &CommandParser::validNoArgOp},
    {"O", &CommandParser::validNoArgOp},
    {"Y", &CommandParser::validOneArgOp},
    {"D", &CommandParser::validOneArgOp},
};