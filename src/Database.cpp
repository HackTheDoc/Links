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
        "'id' INTEGER UNIQUE NOT NULL,"
        "'name' TEXT,"
        "'link' TEXT,"
        "'scam' INTEGER,"
	    "PRIMARY KEY('id'));";
    if (sqlite3_exec(db, table, nullptr, nullptr, nullptr) != SQLITE_OK) {
        Application::Error(sqlite3_errmsg(db));
        return;
    }

    sqlite3_close(db);
}

std::string Database::Get(std::string name) {
    return "";
}

bool Database::Add(std::string name, std::string link, bool scam) {
    return false;
}

bool Database::Remove(std::string name) {
    return false;
}

int Database::TableExistCallback(void* table_exists_ptr, int argc, char** argv, char** colNames) {
    bool* exist = static_cast<bool*>(table_exists_ptr);
    *exist = (argc > 0 && argv[0] != nullptr);
    return 0;
}
