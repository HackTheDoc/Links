#include "include/Database.h"
#include "include/Application.h"

#include <filesystem>
#include <algorithm>

std::string Database::path = "./links-database.db";
sqlite3* Database::db = nullptr;

bool Database::Exist() {
    // check existence
    if (!std::filesystem::exists(path))
        return false;

    // check extension
    if (std::filesystem::path(path).extension() != ".db")
        return false;
    
    // opening the db
    if (sqlite3_open(path.c_str(), &db)) {
        sqlite3_close(db);
        return false;
    }

    // check table existence
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
    
    // check columns
    query = "PRAGMA table_info('links');";
    std::vector<std::string> toCheck = {"id", "name", "link", "scam"};
    std::vector<std::string> columns;
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

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

void Database::Create() {
    sqlite3_open(path.c_str(), &db);

    const char* table =
        "CREATE TABLE IF NOT EXISTS 'links' ( "
        "'id' INTEGER UNIQUE NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "'name' TEXT,"
        "'link' TEXT,"
        "'scam' INTEGER,"
        ");";
    if (sqlite3_exec(db, table, nullptr, nullptr, nullptr) != SQLITE_OK) {
        Application::Error(sqlite3_errmsg(db));
        return;
    }

    sqlite3_close(db);
}

std::string Database::Get(std::string name) {
    sqlite3_open(path.c_str(), &db);

    const char* query = "SELECT links, scam FROM links WHERE name = ?;";
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

bool Database::Add(std::string name, std::string link, bool scam) {
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK)
        return false;

    const char* query = "INSERT INTO links (name, link, scam) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    } 

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, link.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int( stmt, 3, scam);

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
