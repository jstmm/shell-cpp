#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

struct Command {
    std::string original_input;
    std::string executable;
    std::string arguments;
};

Command ParseInput(const std::string &input);

std::string SearchExecutable(const std::string &executable_name, const std::string &env_p);

#endif
