#include "shell.hh"

bool processInput(std::string input_s)
{
    bool running = true;
    auto env_p = std::string(getenv("PATH"));
    Input input = _parseInput(input_s);

    if (input.arguments.contains('>'))
    {
        std::string first_command = input.command;

        for (auto c : input.arguments)
        {
            if (c == '>')
                break;
            first_command += " " + c;
        }
        system(input.original_input.data());
        return true;
    }

    if (input.command == "exit")
        return false;

    if (input.command == "echo")
    {
        std::cout << _echoResult(input.arguments) << '\n';
        return true;
    }

    if (input.command == "type")
    {
        std::cout << _typeResult(input.arguments, env_p) << '\n';
        return true;
    }

    if (input.command == "cd")
    {
        auto result = _changeDirectoryResult(input.command, input.arguments);
        if (result != "")
        {
            std::cout << result << '\n';
        }
        return true;
    }

    if ((_searchExecutable(input.command, env_p) != "") || (input_s == "pwd") || (input.command == "cat"))
    {
        system(input_s.c_str());
        return true;
    }

    std::cout << input.command << ": not found" << '\n';
    return true;
}

Input _parseInput(const std::string_view &input)
{
    bool in_block = false;
    char closing_character = '\0';

    std::vector<std::string> tokens;
    std::string current_token = "";
    bool escape_next = false;

    for (const auto &c : input)
    {
        if (escape_next)
        {
            current_token += c;
            escape_next = false;
            continue;
        }

        if (c == '\\')
        {
            escape_next = true;
            current_token += c;
            continue;
        }

        if (in_block)
        {
            if ((c == '\'' || c == '\"') && c != closing_character && tokens.size() > 0)
            {
                current_token += '\\';
                current_token += c;
                continue;
            }

            if (c == closing_character)
            {
                in_block = false;
                closing_character = '\0';
                current_token += c;
                continue;
            }

            if (c == ' ' && tokens.size() > 0 && closing_character != '\'')
            {
                current_token += '\\';
                current_token += c;
                continue;
            }

            current_token += c;
            continue;
        }

        if (c == ' ')
        {
            if (current_token != "")
            {
                tokens.push_back(current_token);
                current_token = "";
            }
            continue;
        }

        if (c == '\'' || c == '\"')
        {
            in_block = true;
            closing_character = c;
            current_token += c;
            continue;
        }

        current_token += c;
    }
    tokens.push_back(current_token);

    return Input{input, tokens.at(0), join_args(tokens, " "), sliceVector(tokens)};
}

std::string _searchExecutable(const std::string &exec_name, const std::string &env_p)
{
    std::stringstream ss(env_p);
    std::vector<std::string> paths;
    std::string p;
    std::string executable_name;

    if ((exec_name.at(0) == '\'' || exec_name.at(0) == '\"') &&
        (exec_name.at(exec_name.size() - 1) == '\'' || exec_name.at(exec_name.size() - 1) == '\"'))
    {
        executable_name = exec_name.substr(1, exec_name.length() - 2);
    }
    else
    {
        executable_name = exec_name;
    }

    while (std::getline(ss, p, ':'))
    {
        paths.push_back(p);
    }

    for (const auto &path : paths)
    {
        for (const auto &entry : fs::recursive_directory_iterator(path))
        {
            try
            {
                if (entry.is_regular_file() && entry.path().filename() == executable_name)
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
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

    return "";
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

        if (space_found && c == ' ')
            continue;

        if (space_found)
            space_found = false;

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

        if (c == ' ')
            space_found = true;

        if (c == '\\')
        {
            escape_next = true;
            continue;
        }

        result += c;
    }

    return result;
}

std::string _typeResult(const std::string &args, const std::string &env_p)
{
    if ((args == "echo") || (args == "exit") || (args == "type") || (args == "pwd"))
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

std::string _changeDirectoryResult(const std::string &exec, const std::string &args)
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

char _nextCharacter(const char &ch)
{
    return *(&ch + 1);
}

bool _isExcapableCharacter(const char &ch)
{
    return ch == '\\' || ch == '"' || ch == '$' || ch == '\n';
}

std::string join_args(const std::vector<std::string> &vec, const std::string &delimiter)
{
    std::ostringstream result;
    for (size_t i = 1; i < vec.size(); ++i)
    {
        if (i > 1)
        { // Add delimiter after the first element
            result << delimiter;
        }
        result << vec[i];
    }
    return result.str();
}

std::vector<std::string> sliceVector(const std::vector<std::string> &vec)
{
    // Check if the vector has more than 1 element, otherwise return an empty
    // vector
    if (vec.size() <= 1)
    {
        return {}; // Return an empty vector if there are no elements to slice
    }

    // Create a subvector from index 1 to the end
    return std::vector<std::string>(vec.begin() + 1, vec.end());
}
