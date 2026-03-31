#pragma once
#include "pg_connection.hpp"
#include <string>
#include <vector>
#include <optional>

namespace pgcli::db {

// A single row containing strings
struct PGResultRow {
    std::vector<std::optional<std::string>> columns;
};

// Extracted result set to completely hide libpq from other components
struct PGResultSet {
    std::vector<std::string> headers;
    std::vector<PGResultRow> rows;
    
    // Status info
    int affectedRows{0};
    std::string commandStatus;
    
    bool is_empty() const { return rows.empty() && headers.empty(); }
};

class PGQuery {
public:
    explicit PGQuery(PGConnection& connection);

    // Run simple query string
    PGResultSet execute(const std::string& sql);
    
    // Support for parameterized queries (text format)
    PGResultSet executeParams(const std::string& sql, const std::vector<std::string>& params);

private:
    PGConnection& connection_;
    PGResultSet processResult(PGresult* res);
};

} // namespace pgcli::db
