#include "command.hpp"

char nextCharacter(const char &ch)
{
    return *(&ch + 1);
}

bool isExcapableCharacter(const char &ch)
{
    return  ch == '\\' ||
            ch == '"' ||
            ch == '$'  ||
            ch == '\n' ;
}

std::string echoResult(const std::string &params)
{
    std::string result = "";
    bool in_single_quote = false;
    bool in_double_quote = false;
    bool space_found = false;
    bool escape_next = false;

    for (const auto &c : params)
    {
        if (in_single_quote)
        {
            if (c == '\'')
            {
                in_single_quote = false;
                continue;
            }
            result += c;
        }
        else
        {
            if (escape_next)
            {
                escape_next = false;
                result += c;
                continue;
            }

            if (in_double_quote)
            {
                if (c == '"')
                {
                    in_double_quote = false;
                    continue;
                }

                if (c == '\\' && isExcapableCharacter(nextCharacter(c)))
                {
                    escape_next = true;
                    continue;
                }
            }
            else
            {
                if (space_found && c == ' ') continue;
                if (space_found)             space_found = false;
                
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
                {
                    space_found = true;
                }

                if (c == '\\')
                {
                    escape_next = true;
                    continue;
                }
            }

            result += c;
        }
    }

    return result;
}

std::string typeResult(const std::string &args, const std::string &env_p)
{
    if ((args == "echo") || (args == "exit") || (args == "type") || (args == "pwd"))
    {
        return args + " is a shell builtin";
    }

    std::string exec_path = SearchExecutable(args, env_p);

    if (exec_path == "")
    {
        return args + ": not found";
    }

    return args + " is " + exec_path;
}

std::string changeDirectoryResult(const std::string &exec, const std::string &args)
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

int main()
{
    std::string env_p = std::string(getenv("PATH"));

    while (true)
    {
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;

        std::string input;
        std::cout << "$ ";
        std::getline(std::cin, input);

        Command command = ParseInput(input);
        std::string exec = command.executable, args = command.arguments;

        if (exec == "exit")
            return 0;

        if (exec == "echo")
        {
            std::cout << echoResult(args) << std::endl;
            continue;
        }

        if (exec == "type")
        {
            std::cout << typeResult(args, env_p) << std::endl;
            continue;
        }

        if (exec == "cd")
        {
            std::string result = changeDirectoryResult(exec, args);
            if (result != "")
            {
                std::cout << result << std::endl;
            }
            continue;
        }

        if ((SearchExecutable(exec, env_p) != "") || (input == "pwd"))
        {
            system(input.c_str());
            continue;
        }

        std::cout << exec << ": not found" << std::endl;
    }
}
