#include "shell.hh"

int main()
{
    bool running = true;

    while (running)
    {
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;

        std::string input;
        std::print("$ ");
        std::getline(std::cin, input);

        running = processInput(input);
    }

    return 0;
}
