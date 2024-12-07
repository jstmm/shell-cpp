#include "command.hpp"

namespace fs = std::filesystem;

Command ParseInput(const std::string &input)
{
    bool exec_found = false;
    bool args_found = false;

    std::string exec = "", args = "";

    char closing_character = 0;

    for (const auto &c : input)
    {
        if (exec_found && c == ' ' && !args_found)
        {
            continue;
        }

        if (exec_found)
        {
            if (!args_found) args_found = true;
            args += c;
            continue;
        }

        if (c == ' ' && !closing_character){
            exec_found = true;
            continue;
        }

        if ((c == '\'' || c == '"') && !closing_character)
        {
            closing_character = c;
            continue;
        }

        if (c == closing_character)
        {
            exec_found = true;
            continue;
        }

        exec += c;
    }

    return Command {input, exec, args};
}

std::string SearchExecutable(const std::string &executable_name, const std::string &env_p)
{
    std::stringstream ss(env_p);
    std::vector<std::string> paths;
    std::string p;

    while (std::getline(ss, p, ':'))
    {
        paths.push_back(p);
    }

    for (const auto &path : paths)
    {
        for (const auto &entry : fs::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file() &&
                entry.path().filename() == executable_name)
            {
                auto perms = entry.status().permissions();
                if ((perms & fs::perms::owner_exec) != fs::perms::none ||
                    (perms & fs::perms::group_exec) != fs::perms::none ||
                    (perms & fs::perms::others_exec) != fs::perms::none)
                {
                    return entry.path().c_str();
                }
            }
        }
    }

    return "";
}
