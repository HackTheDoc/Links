#include <iostream>
#include <docopt/docopt.h>

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
    std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                        {argv + 1, argv + argc},
                        true,
                        "links 0.1");

    app.start();

    while (Application::isRunning) app.run();

    app.kill();

    return 0;
}
