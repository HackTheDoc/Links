#include "include/Application.h"
#include "include/Database.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

bool Application::isRunning = false;

const std::map<std::string, Application::Command> Application::STRING_TO_COMMAND {
    {"unknown"  , Application::Command::UNKNOWN },
    {"help"     , Application::Command::HELP    },
    {"exit"     , Application::Command::EXIT    },
    {"clear"    , Application::Command::CLEAR   },

    {"list"     , Application::Command::LIST    },
    {"ls"       , Application::Command::LIST    },
    {"get"      , Application::Command::GET     },
    {"new"      , Application::Command::NEW     },
    {"remove"   , Application::Command::REMOVE  },
    {"rm"       , Application::Command::REMOVE  }
};

Application::Application() {}

Application::~Application() {}

void Application::start() {
    // init cmds
    commandClear();

    std::cout << "try out ";
    std::cout << "\033[33m" << "help" << "\033[0m";
    std::cout << " command!" << std::endl;

    // init db
    if (!Database::Exist()) Database::Create();
    
    isRunning = true;
}

void Application::run() {
    parseInput();
 
    if (buffer.size() > 0) eval(buffer.at(0));
}

void Application::kill() {
    // nothing to kill for now
}

Application::Command Application::convertToCommand(std::string s) {
    if (STRING_TO_COMMAND.count(s) <= 0) return Application::Command::UNKNOWN;
    else return STRING_TO_COMMAND.at(s);
}

void Application::eval(std::string input) {
    Application::Command c = convertToCommand(input);

    switch (c) {
    case Application::Command::HELP:
        commandHelp();
        break;
    case Application::Command::EXIT:
        commandExit();
        break;
    case Application::Command::CLEAR:
        commandClear();
        break;
    case Application::Command::LIST:
        commandList();
        break;
    case Application::Command::GET:
        commandGet();
        break;
    case Application::Command::NEW:
        commandNew();
        break;
    case Application::Command::REMOVE:
        commandRemove();
        break;
    case Application::Command::UNKNOWN:
    default:
        Error("command \""+buffer.at(0)+"\" is unkown");
        break;
    }
}

void Application::parseInput() {
    buffer.clear();

    std::cout << "\033[32m";
    std::cout << "> ";
    std::cout << "\033[0m";

    std::string raw;
    std::getline(std::cin, raw);

    std::stringstream ss(raw);
    std::string word;
    bool insideQuote = false;

    while (getline(ss, word, ' ')) {
        if (!insideQuote) {
            if (word.front() == '"' || word.front() == '\'') {
                // If an opening quote is found, set the flag and remove the quote character
                insideQuote = true;
                word = word.substr(1);
            }
            buffer.push_back(word);
        }
        else {
            if (word.back() == '"' || word.back() == '\'') {
                // If a closing quote is found, reset the flag and remove the quote character
                insideQuote = false;
                word.pop_back();
                buffer.back() += ' ' + word; // Append to the last word in the buffer
            } else {
                // If we are inside a quote, keep appending to the last word in the buffer
                buffer.back() += ' ' + word;
            }
        }
    }
}

void Application::Error(std::string e) {
    std::cout << "\033[31m";
    std::cout << "ERROR: ";
    std::cout << "\033[0m";
    std::cout << e << std::endl;
}

void Application::Warning(std::string w) {
    std::cout << "\033[33m";
    std::cout << "WARNING: ";
    std::cout << "\033[0m";
    std::cout << w << std::endl;
}

void Application::Log(std::string l) {
    std::cout << "\033[34m";
    std::cout << "LOG: ";
    std::cout << "\033[0m";
    std::cout << l << std::endl;
}

void Application::SetClipboardText(const std::string& text) {
    #ifdef _WIN32
        return;
    #elif __APPLE__
        return;
    #elif __linux__
        FILE* pipe = popen("xclip -selection clipboard", "w");
        if (!pipe) {
            return;
        }

        fwrite(text.c_str(), 1, text.size(), pipe);
        pclose(pipe);
    #endif
}

void Application::commandHelp() {
    std::cout << "Usage:"                                                               << std::endl;
    std::cout << "  <command> [options]"                                                << std::endl;

    std::cout << "Command:"                                                             << std::endl;
    std::cout << "  help                        Show this screen"                       << std::endl;
    std::cout << "  help <command>              Show help of a given command"           << std::endl;
    std::cout << "  exit                        Quit the application"                   << std::endl;
    std::cout << "  clear                       Clear the terminal"                     << std::endl;
    std::cout << "  list                        List all known name"                    << std::endl;
    std::cout << "  get <name>                  Return a referenced link if founded"    << std::endl;
    std::cout << "  new <name> <link> [scam]    Register a new link"                    << std::endl;
    std::cout << "  remove <name>               Remove a referenced link"               << std::endl;
}

void Application::commandExit() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    
    isRunning = false;
}

void Application::commandClear() {
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    
    std::cout << " ______________________________" << std::endl;
    std::cout << "|  _     ___ _   _ _  ______   |" << std::endl;
    std::cout << "| | |   |_ _| \\ | | |/ / ___|  |" << std::endl;
    std::cout << "| | |    | ||  \\| | ' /\\___ \\  |" << std::endl;
    std::cout << "| | |___ | || |\\  | . \\ ___) | |" << std::endl;
    std::cout << "| |_____|___|_| \\_|_|\\_\\____/  |" << std::endl;
    std::cout << "|______________________________|" << std::endl;

}

void Application::commandList() {
    std::vector<std::string> names = Database::List();

    for (auto n : names)
        std::cout << n << std::endl;
}

void Application::commandGet() {
    if (buffer.size() == 1) {
        Error("missing name");
        return;
    }

    std::string name = buffer.at(1);
    
    std::string link = Database::Get(name);

    if (link.size() == 0) {
        Error("unknown name \""+name+"\"");
        return;
    }

    bool scam = false;
    if (link[0] == '$') {
        scam = true;
        link.erase(link.begin());
    }

    if (scam)
        Warning("following link marked as scam");
    std::cout << link << std::endl;
    SetClipboardText(link);
}

void Application::commandNew() {
    if (buffer.size() < 3) {
        Error("missing arguments (name and/or link)");
        return;
    }

    std::string name = buffer.at(1);
    std::string link = buffer.at(2);
    bool scam = false;
    if (buffer.size() >= 4) {
        std::string s = buffer.at(3);
        if (s == "true" || s == "0" || s == "t")
            scam = true;
    }

    bool success = Database::Add(name, link, scam);
    if (!success)
        Error("link referencing failed");
}

void Application::commandRemove() {
    if (buffer.size() == 1) {
        Error("missing name");
        return;
    }

    std::string name = buffer.at(1);
    std::string link = Database::Get(name);
    bool success = Database::Remove(name);

    if (!success) {
        Error("unknown error happened while trying to remove \""+name+"\"");
        return;
    }

    std::cout << "successfully removed " << link << std::endl;
}
