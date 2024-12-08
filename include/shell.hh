#pragma once

#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <vector>
#include <print>
#include <ranges>

namespace fs = std::filesystem;

struct Command
{
    std::string_view original_input;
    std::string executable;
    std::string arguments;
};

bool processInput(std::string input);
