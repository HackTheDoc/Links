#include <iostream>

#include "include/Application.h"

Application app;

const char* USAGE =
R"(Links.

    Usage:
        links
        links --version
    
    Options:
        --version Show version.
)";

int main(int argc, const char* argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-v" || arg == "--version") {
            std::cout << "links " + Application::version << std::endl;
            return 0;
        }
        else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage:"                                   << std::endl;
            std::cout << "  links [option]"                         << std::endl;

            std::cout << std::endl;

            std::cout << "Options:"                                 << std::endl;
            std::cout << "  -h or --help    Display this text."     << std::endl;
            std::cout << "  -v or --version Show current version."  << std::endl;

            return 0;
        }
    }

    app.start();

    while (Application::isRunning) app.run();

    app.kill();

    return 0;
}
