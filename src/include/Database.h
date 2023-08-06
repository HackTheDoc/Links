#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

class Database {
public:
    static std::string path;
    static sqlite3* db;

    static bool Exist();

    static void Create();
    static std::vector<std::string> List();
    static int Size();
    static std::string Get(std::string name);
    static bool Add(std::string name, std::string link, bool scam);
    static bool Remove(std::string name);
    static void SetLink(std::string name, std::string link);
    static void SetScam(std::string name, bool scam);
};
