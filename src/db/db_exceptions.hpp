#pragma once
#include <stdexcept>
#include <string>

namespace pgcli::db {

class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& msg) : std::runtime_error("Database Error: " + msg) {}
};

class ConnectionException : public DatabaseException {
public:
    explicit ConnectionException(const std::string& msg) : DatabaseException("Connection Error: " + msg) {}
};

class QueryException : public DatabaseException {
public:
    explicit QueryException(const std::string& msg) : DatabaseException("Query Error: " + msg) {}
};

} // namespace pgcli::db
