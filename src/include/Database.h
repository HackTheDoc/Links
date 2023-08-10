#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

class Database {
public:
    static std::string path;
    static sqlite3* db;

    static void SetPath(std::string p);

    static bool Exist();

    static void Create();
    static std::vector<std::string> List(bool chatroom, bool forum, bool library, bool scam, bool wiki);
    static int Size();
    static std::string Get(std::string name);
    static bool Add(std::string name, std::string link, bool chatroom, bool forum, bool library, bool scam, bool wiki);
    static bool Remove(std::string name);
    static void SetLink(std::string name, std::string link);
    static void SetChatroom(std::string name, bool scam);
    static void SetForum(std::string name, bool scam);
    static void SetLibrary(std::string name, bool scam);
    static void SetScam(std::string name, bool scam);
    static void SetWiki(std::string name, bool scam);
};
