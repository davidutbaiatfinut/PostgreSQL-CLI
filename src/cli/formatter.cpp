#include "formatter.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>

namespace pgcli::cli {

void Formatter::printTable(const db::PGResultSet& resultSet) const {
    if (resultSet.is_empty()) {
        if (!resultSet.commandStatus.empty()) {
            std::cout << resultSet.commandStatus << "\n";
        }
        return;
    }

    std::vector<size_t> colWidths;
    for (const auto& header : resultSet.headers) {
        colWidths.push_back(std::max<size_t>(header.length(), 4));
    }

    for (const auto& row : resultSet.rows) {
        for (size_t i = 0; i < row.columns.size(); ++i) {
            size_t cellLen = row.columns[i].has_value() ? row.columns[i]->length() : 4; 
            if (cellLen > colWidths[i]) {
                colWidths[i] = cellLen;
            }
        }
    }

    for (size_t i = 0; i < resultSet.headers.size(); ++i) {
        std::cout << formatCell(resultSet.headers[i], colWidths[i]) << (i < resultSet.headers.size() - 1 ? " | " : "");
    }
    std::cout << "\n";

    for (size_t i = 0; i < resultSet.headers.size(); ++i) {
        std::cout << std::string(colWidths[i], '-') << (i < resultSet.headers.size() - 1 ? "-+-" : "");
    }
    std::cout << "\n";

    for (const auto& row : resultSet.rows) {
        for (size_t i = 0; i < row.columns.size(); ++i) {
            std::string text = row.columns[i].has_value() ? *row.columns[i] : "NULL";
            std::cout << formatCell(text, colWidths[i]) << (i < row.columns.size() - 1 ? " | " : "");
        }
        std::cout << "\n";
    }

    if (!resultSet.commandStatus.empty()) {
        std::cout << resultSet.commandStatus << "\n";
    } else {
        std::cout << "(" << resultSet.rows.size() << " rows)\n";
    }
}

std::string Formatter::formatCell(const std::string& text, size_t width) const {
    std::string formatted = text;
    if (formatted.length() < width) {
        formatted.append(width - formatted.length(), ' ');
    }
    return formatted;
}

void Formatter::exportToCSV(const db::PGResultSet& resultSet, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filepath);
    }

    for (size_t i = 0; i < resultSet.headers.size(); ++i) {
        file << "\"" << resultSet.headers[i] << "\"" << (i < resultSet.headers.size() - 1 ? "," : "");
    }
    file << "\n";

    for (const auto& row : resultSet.rows) {
        for (size_t i = 0; i < row.columns.size(); ++i) {
            std::string text = row.columns[i].has_value() ? *row.columns[i] : "";
            size_t pos = 0;
            // Escape quotes inside CSV fields
            while ((pos = text.find("\"", pos)) != std::string::npos) {
                text.replace(pos, 1, "\"\"");
                pos += 2;
            }
            file << "\"" << text << "\"" << (i < row.columns.size() - 1 ? "," : "");
        }
        file << "\n";
    }
}

} // namespace pgcli::cli
