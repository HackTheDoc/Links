#include <iostream>
#include <docopt/docopt.h>

#include "include/Application.h"

Application app;

std::string version = "1";

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
                        "links "+version+".0");

    app.start("1");

    while (Application::isRunning) app.run();

    app.kill();

    return 0;
}
