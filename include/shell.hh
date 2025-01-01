#pragma once

#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Input
{
    std::string_view original_input;
    std::string command;
    std::string arguments;
    std::vector<std::string> args;
};

bool processInput(std::string input);

Input _parseInput(const std::string_view &input);
std::string _searchExecutable(const std::string &exec_name, const std::string &env_p);
std::string _echoResult(const std::string &params);
std::string _typeResult(const std::string &args, const std::string &env_p);
std::string _changeDirectoryResult(const std::string &exec, const std::string &args);
char _nextCharacter(const char &ch);
bool _isExcapableCharacter(const char &ch);
std::string join_args(const std::vector<std::string> &vec, const std::string &delimiter);
std::vector<std::string> sliceVector(const std::vector<std::string> &vec);
