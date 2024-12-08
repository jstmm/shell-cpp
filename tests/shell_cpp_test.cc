#include <gtest/gtest.h>

#include "shell.hh"

TEST(ShellCppTest, Shell_ProcessInput_HandleInvalidCommands)
{
    // Arrange
    std::streambuf *originalCoutBuffer = std::cout.rdbuf();
    std::stringstream output;
    std::cout.rdbuf(output.rdbuf());

    Shell shell;

    // Act
    shell.processInput("invalid_pineapple_command");

    // Assert
    EXPECT_EQ(output.str(), "invalid_pineapple_command: not found");

    std::cout.rdbuf(originalCoutBuffer);
}

TEST(ShellCppTest, BasicAssertions2)
{
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);
}
