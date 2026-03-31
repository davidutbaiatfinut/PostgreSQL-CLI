#pragma once
#include "db/pg_query.hpp"
#include <string>

namespace pgcli::cli {

class Formatter {
public:
    void printTable(const db::PGResultSet& resultSet) const;
    void exportToCSV(const db::PGResultSet& resultSet, const std::string& filepath) const;

private:
    std::string formatCell(const std::string& text, size_t width) const;
};

} // namespace pgcli::cli
