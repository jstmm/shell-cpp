#include "shell.hh"

Command _parseInput(const std::string_view &input)
{
    bool exec_found = false;
    bool args_found = false;

    std::string exec = "", args = "";

    char closing_character = 0;

    for (const auto &c : input)
    {
        if (exec_found && c == ' ' && !args_found)
            continue;

        if (exec_found)
        {
            if (!args_found)
                args_found = true;
            args += c;
            continue;
        }

        if (c == ' ' && !closing_character)
        {
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

    return Command{input, exec, args};
}

char _nextCharacter(const char &ch) { return *(&ch + 1); }

bool _isExcapableCharacter(const char &ch)
{
    return ch == '\\' || ch == '"' || ch == '$' || ch == '\n';
}

std::string _echoResult(const std::string &params)
{
    std::string result = "";
    bool in_single_quote = false;
    bool in_double_quote = false;
    bool space_found = false;
    bool escape_next = false;

    for (const auto &c : params)
    {
        if (in_single_quote && (c == '\''))
        {
            in_single_quote = false;
            continue;
        }

        if (in_single_quote)
        {
            result += c;
            continue;
        }

        if (escape_next)
        {
            escape_next = false;
            result += c;
            continue;
        }

        if (in_double_quote && (c == '"'))
        {
            in_double_quote = false;
            continue;
        }

        if (in_double_quote && (c == '\\') && _isExcapableCharacter(_nextCharacter(c)))
        {
            escape_next = true;
            continue;
        }

        if (space_found && c == ' ') continue;

        if (space_found) space_found = false;

        if (c == '\'')
        {
            in_single_quote = true;
            continue;
        }

        if (c == '"')
        {
            in_double_quote = true;
            continue;
        }

        if (c == ' ') space_found = true;

        if (c == '\\')
        {
            escape_next = true;
            continue;
        }

        result += c;
    }

    return result;
}

std::string _searchExecutable(const std::string &executable_name, const std::string &env_p)
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

std::string _typeResult(const std::string &args, const std::string &env_p)
{
    if ((args == "echo") || (args == "exit") || (args == "type") ||
        (args == "pwd"))
    {
        return args + " is a shell builtin";
    }

    auto exec_path = _searchExecutable(args, env_p);

    if (exec_path.empty())
    {
        return args + ": not found";
    }
    return args + " is " + exec_path;
}

std::string _changeDirectoryResult(const std::string &exec,
                                   const std::string &args)
{
    if (args == "~")
    {
        chdir(getenv("HOME"));
        return "";
    }

    if (fs::exists(fs::path(args)))
    {
        chdir(args.c_str());
        return "";
    }

    return exec + ": " + args + ": No such file or directory";
}

bool processInput(std::string input)
{
    bool running = true;

    std::string env_p = std::string(getenv("PATH"));
    Command command = _parseInput(input);
    std::string exec = command.executable, args = command.arguments;

    if (exec == "exit") return false;

    if (exec == "echo")
    {
        std::println("{}", _echoResult(args));
        return true;
    }

    if (exec == "type")
    {
        std::println("{}", _typeResult(args, env_p));
        return true;
    }

    if (exec == "cd")
    {
        auto result = _changeDirectoryResult(exec, args);
        if (result != "")
        {
            std::println("{}", result);
        }
        return true;
    }

    if ((_searchExecutable(exec, env_p) != "") || (input == "pwd"))
    {
        system(input.c_str());
        return true;
    }

    std::println("{}: not found", exec);
    return true;
}
