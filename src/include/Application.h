#pragma once

#include <string>
#include <vector>
#include <map>

class Application {
public:
    enum Command {
        UNKNOWN,
        HELP,
        EXIT,
        CLEAR,

        UPGRADE,
        LIST,
        SIZE,

        GET,
        NEW,
        REMOVE,
        SET
    };

    static const std::string version;
    static bool isRunning;

    Application();
    ~Application();

    /// @brief Initialize the application (check database existence)
    void start();
    /// @brief Keep the application running
    void run();
    /// @brief Kill running proccesses
    void kill();

    void eval(std::string input);

    /// @brief Display an error
    /// @param e error message
    static void Error(std::string e);
    /// @brief Display a warning
    /// @param w warning message
    static void Warning(std::string w);
    /// @brief Display a log
    /// @param l log message
    static void Log(std::string l);
    static void SetClipboardText(const std::string& text);

private:
    static const std::map<std::string, Application::Command> STRING_TO_COMMAND;

    std::vector<std::string> buffer;

    /// @brief Get the command associated with the given string
    /// @param s string to convert
    /// @return the associated command or UNKNOWN if not found
    Application::Command convertToCommand(std::string s);

    /// @brief parse input for user commands
    void parseInput();

    // all the functions associated to a specific command

    void commandHelp();
    void commandExit();
    void commandClear();

    void commandList();
    void commandSize();

    void commandGet();
    void commandNew();
    void commandRemove();
    void commandSet();

    void commandUpgrade();
};
