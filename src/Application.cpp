#include "include/Application.h"
#include "include/Database.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

const std::string Application::version = "2.0";
bool Application::isRunning = false;

const std::map<std::string, Application::Command> Application::STRING_TO_COMMAND {
    {"unknown"  , Application::Command::UNKNOWN },
    {"help"     , Application::Command::HELP    },
    {"exit"     , Application::Command::EXIT    },
    {"clear"    , Application::Command::CLEAR   },

    {"upgrade"  , Application::Command::UPGRADE },
    {"list"     , Application::Command::LIST    },
    {"ls"       , Application::Command::LIST    },
    {"size"     , Application::Command::SIZE    },
    {"sz"       , Application::Command::SIZE    },

    {"get"      , Application::Command::GET     },
    {"new"      , Application::Command::NEW     },
    {"remove"   , Application::Command::REMOVE  },
    {"rm"       , Application::Command::REMOVE  },
    {"set"      , Application::Command::SET     }
};

Application::Application() {}

Application::~Application() {}

void Application::start() {
    // init db
    const char* homeDir = std::getenv("HOME");
    if (homeDir) {
        std::string home(homeDir);
        Database::SetPath(home+"/.links/");
    }
    else
        Database::SetPath("/tmp/links.db");

    if (!Database::Exist()) Database::Create();

    // init cmds
    commandClear();

    std::cout << "try out ";
    std::cout << "\033[33m" << "help" << "\033[0m";
    std::cout << " command!" << std::endl;
    
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
    case Application::Command::UPGRADE:
        commandUpgrade();
        break;
    case Application::Command::LIST:
        commandList();
        break;
    case Application::Command::SIZE:
        commandSize();
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
    case Application::Command::SET:
        commandSet();
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
            if (word.back() == '"') {
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
    if (buffer.size() > 1) {
        Application::Command c = convertToCommand(buffer.at(1));

        switch (c) {
        case Application::Command::HELP:
            std::cout << "help              List of commands"                           << std::endl;
            std::cout << "help <command>    Display help of a specific command"         << std::endl;
            break;
        case Application::Command::EXIT:
            std::cout << "exit              Close the application"                      << std::endl;
            break;
        case Application::Command::CLEAR:
            std::cout << "clear             Clear the terminal"                         << std::endl;
            break;
        case Application::Command::UPGRADE:
            std::cout << "upgrade           Upgrade an old db to a new one"             << std::endl;
            break;
        case Application::Command::LIST:
            std::cout << "list              List every link registered (their name)"    << std::endl;
            std::cout << "list chatroom     List every link to a chatroom"              << std::endl;
            std::cout << "list forum        List every link to a forum"                 << std::endl;
            std::cout << "list library      List every link to a library"               << std::endl;
            std::cout << "list wiki         List every link to a forum"                 << std::endl;
            break;
        case Application::Command::SIZE:
            std::cout << "size              Current size of the dataase"                << std::endl;
            break;
        case Application::Command::GET:
            std::cout << "get <name>        Get a link by it's name"                    << std::endl;
            std::cout << "                  -> copied in clipboard"                     << std::endl;
            break;
        case Application::Command::NEW:
            std::cout << "new <name> <link> [options] Add a new link with possible options" << std::endl;
            std::cout << std::endl;
            std::cout << "Warning:" << std::endl;
            std::cout << "option must be like '00000' where each 0 can be replaced by a 1 and stand for true or false." << std::endl;
            std::cout << "The flag order is : [chatroom, forum, library, scam, wiki]" << std::endl;
            break;
        case Application::Command::REMOVE:
            std::cout << "remove <name>     Remove an existing link from the database"  << std::endl;
            break;
        case Application::Command::SET:
            std::cout << "set <name> -l <link>  Set the link to a new one"              << std::endl;
            std::cout << "           -s <scam>  Set wether the link is a scam or not"   << std::endl;
            break;
        case Application::Command::UNKNOWN:
        default:
            break;
        }

        if (c != Application::Command::UNKNOWN)
            return;
    }

    std::cout << "Usage:"                       << std::endl;
    std::cout << "  <command> [options]"        << std::endl;

    std::cout << std::endl;

    std::cout << "Command:"                     << std::endl;
    std::cout << "  help"                       << std::endl;
    std::cout << "  help <command>"             << std::endl;
    std::cout << "  exit"                       << std::endl;
    std::cout << "  clear"                      << std::endl;

    std::cout << std::endl;

    std::cout << "  list"                       << std::endl;
    std::cout << "  size"                       << std::endl;
    
    std::cout << std::endl;

    std::cout << "  get <name>"                 << std::endl;
    std::cout << "  new <name> <link> [scam]"   << std::endl;
    std::cout << "  remove <name>"              << std::endl;
    std::cout << "  set <name> -l <link>"       << std::endl;
    std::cout << "             -s <scam>"       << std::endl;

    std::cout << std::endl;

    std::cout << "  upgrade"                    << std::endl;
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

void Application::commandUpgrade() {
    Warning("not yet implemented");
}

void Application::commandList() {
    std::vector<std::string> names;

    if (buffer.size() > 1) {
        std::string p = buffer[1];

        if (p == "chatroom")
            names = Database::List(true , false, false, false, false);
        else if (p == "forum")
            names = Database::List(false, true , false, false, false);
        else if (p == "library")
            names = Database::List(false, false, true , false, false);
        else if (p == "scam")
            names = Database::List(false, false, false, true , false);
        else if (p == "wiki")
            names = Database::List(false, false, false, false, true );
        else
            Warning("unknown parameter \""+p+"\"");
    }
    else {
        names = Database::List(false, false, false, false, false);
    }

    for (auto n : names)
        std::cout << n << std::endl;
}

void Application::commandSize() {
    std::cout << Database::Size() << std::endl;
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

    bool success = Database::Add(name, link, false, false, false, scam, false);
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
    if (link[0] == '$')
        link.erase(link.begin());

    bool success = Database::Remove(name);

    if (!success) {
        Error("unknown error happened while trying to remove \""+name+"\"");
        return;
    }

    std::cout << "successfully removed " << link << std::endl;
}

void Application::commandSet() {
    if (buffer.size() < 4) {
        Error("missing parameter(s)");
        return;
    }

    std::string name = buffer.at(1);

    if (Database::Get(name) == "") {
        Error("unknown name \""+name+"\"");
        return;
    }

    std::string cmd = buffer.at(2);

    if (cmd == "-l") {
        std::string link = buffer.at(3);
        
        Database::SetLink(name, link);
    }
    else if (cmd == "-s") {
        std::string s = buffer.at(3);

        if (s == "true" || s == "0" || s == "t")
            Database::SetScam(name, true);
        else if (s == "false" || s == "1" || s == "f")
            Database::SetScam(name, false);
        else
            Error("invalid value \""+s+"\"");
    }
    else {
        Error("unknown flag \""+cmd+"\". Please use \"-l\" or \"-s\".");
    }
}
