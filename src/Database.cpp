#include "include/Database.h"
#include "include/Application.h"

#include <filesystem>
#include <algorithm>

#ifdef _WIN32 // Windows
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
#else // POSIX (Linux, macOS, etc.)
    #include <sys/stat.h>
    #define MKDIR(dir) mkdir(dir, 0777)
#endif

std::string Database::path = "/tmp/.links/";
sqlite3* Database::db = nullptr;

void Database::SetPath(std::string p) {
    path = p;
}

bool Database::Exist() {
    // check existence
    if (!std::filesystem::exists(path))
        return false;

    // check extension
    if (std::filesystem::path(path).extension() != ".db")
        return false;
    
    if (!HaveLinksTable(path) || !HaveCorrectParams(path))
        return false;

    return true;
}

void Database::Create() {
    if (path != "/tmp/links.db")
        MKDIR(path.c_str());
    path += "database-"+Application::version+".db";

    sqlite3_open(path.c_str(), &db);

    const char* table =
        "CREATE TABLE IF NOT EXISTS 'links' ( "
        "'id' INTEGER UNIQUE NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "'name' TEXT,"
        "'link' TEXT,"
        "'chatroom' INTEGER,"
        "'forum' INTEGER,"
        "'library' INTEGER,"
        "'scam' INTEGER,"
        "'wiki' INTEGER"
        ");";
    if (sqlite3_exec(db, table, nullptr, nullptr, nullptr) != SQLITE_OK) {
        Application::Error(sqlite3_errmsg(db));
        return;
    }

    sqlite3_close(db);
}

void Database::Copy(std::string p, int v) {
    switch (v) {
    case 1:
        Copy_V1(p);
        break;
    default:
        Application::Error("nothing to update");
        break;
    }
}

std::vector<std::string> Database::List(bool chatroom, bool forum, bool library, bool scam, bool wiki) {
    std::vector<std::string> buffer;

    sqlite3_open(path.c_str(), &db);

    std::string query = "SELECT name FROM links ";
    if (chatroom)
        query += "WHERE chatroom = ? ";
    else if (forum)
        query += "WHERE forum = ? ";
    else if (library)
        query += "WHERE library = ? ";
    else if (scam)
        query += "WHERE scam = ? ";
    else if (wiki)
        query += "WHERE wiki = ? ";
        
    query += "ORDER BY name ASC;";
    
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (chatroom)
        sqlite3_bind_int(stmt, 1, chatroom);
    else if (forum)
        sqlite3_bind_int(stmt, 1, forum);
    else if (library)
        sqlite3_bind_int(stmt, 1, library);
    else if (scam)
        sqlite3_bind_int(stmt, 1, scam);
    else if (wiki)
        sqlite3_bind_int(stmt, 1, wiki);

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        buffer.push_back((const char*)sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);

    sqlite3_close(db);

    return buffer;
}

int Database::Size() {
    int size = 0;

    sqlite3_open(path.c_str(), &db);

    const char* query = "SELECT COUNT(*) FROM links;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        size = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    sqlite3_close(db);

    return size;
}

std::string Database::Get(std::string name) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "SELECT link, scam FROM links WHERE name = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW || rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "";
    }

    std::string data = (const char*)sqlite3_column_text(stmt, 0);
    
    int scam = sqlite3_column_int(stmt, 1);
    if (scam) data.insert(data.begin(), '$'); 

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return data;
}

bool Database::Insert(std::string name, std::string link, bool chatroom, bool forum, bool library, bool scam, bool wiki) {
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;

    const char* query = "INSERT INTO links (name, link, chatroom, forum, library, scam, wiki) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    } 

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, link.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int( stmt, 3, chatroom);
    sqlite3_bind_int( stmt, 4, forum);
    sqlite3_bind_int( stmt, 5, library);
    sqlite3_bind_int( stmt, 6, scam);
    sqlite3_bind_int( stmt, 7, wiki);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

bool Database::Remove(std::string name) {
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;
    
    const char* query = "DELETE FROM links WHERE name = ?;";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    } 

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

void Database::SetLink(std::string name, std::string link) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "UPDATE links SET link = ? WHERE name = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, link.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        Application::Error("failed to update the link");
    }
    
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void Database::SetChatroom(std::string name, bool value) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "UPDATE links SET chatroom = ? WHERE name = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    sqlite3_bind_int( stmt, 1, value);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        Application::Error("failed to update the chatroom flag");
    }
    
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void Database::SetForum(std::string name, bool value) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "UPDATE links SET forum = ? WHERE name = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    sqlite3_bind_int( stmt, 1, value);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        Application::Error("failed to update the forum flag");
    }
    
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void Database::SetLibrary(std::string name, bool value) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "UPDATE links SET library = ? WHERE name = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    sqlite3_bind_int( stmt, 1, value);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        Application::Error("failed to update the library flag");
    }
    
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void Database::SetScam(std::string name, bool value) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "UPDATE links SET scam = ? WHERE name = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    sqlite3_bind_int( stmt, 1, value);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        Application::Error("failed to update the scam flag");
    }
    
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

void Database::SetWiki(std::string name, bool value) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "UPDATE links SET wiki = ? WHERE name = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    sqlite3_bind_int( stmt, 1, value);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        Application::Error("failed to update the wiki flag");
    }
    
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

bool Database::HaveLinksTable(std::string p) {
    if (sqlite3_open(p.c_str(), &db)) {
        sqlite3_close(db);
        return false;
    }

    const char* query = "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = 'links';";
    sqlite3_stmt* stmt;
    bool flag = false;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            flag = true;
        }
        sqlite3_finalize(stmt);
    }
    if (!flag) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);

    return true;
}

bool Database::HaveCorrectParams(std::string p) {
    if (sqlite3_open(p.c_str(), &db)) {
        sqlite3_close(db);
        return false;
    }

    const char* query = "PRAGMA table_info('links');";
    sqlite3_stmt* stmt;
    std::vector<std::string> toCheck = {"id", "name", "link", "chatroom", "forum", "library", "scam", "wiki"};
    std::vector<std::string> columns;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc == SQLITE_OK) {
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            const unsigned char* c = sqlite3_column_text(stmt, 1);
            if (c != nullptr) {
                columns.push_back(reinterpret_cast<const char*>(c));
            }
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);

    if (columns.size() != toCheck.size()) return false;
    for (int i = 0; i < (int)toCheck.size(); i++) {
        if (columns[i] != toCheck[i])
            return false;
    }
    return true;
}

void Database::Copy_V1(std::string p) {
    if (!HaveLinksTable(p)) {
        Application::Error("located a database at \""+p+"\" but table 'links' not found");
        return;
    }

    sqlite3_open(p.c_str(), &db);

    const char* query = "SELECT name, link, scam FROM links";
    sqlite3_stmt* stmt;
    sqlite3_prepare(db, query, -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string name = (const char*)sqlite3_column_text(stmt, 0);
        std::string link = (const char*)sqlite3_column_text(stmt, 1);
        int scam = sqlite3_column_int(stmt, 2);

        if (Get(name) != "")
            continue;
        else
            Insert(name, link, false, false, false, scam, false);
    }

    sqlite3_close(db);
}
