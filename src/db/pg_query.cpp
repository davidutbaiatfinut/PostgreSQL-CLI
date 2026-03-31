#include "pg_query.hpp"
#include "db_exceptions.hpp"

namespace pgcli::db {

PGQuery::PGQuery(PGConnection& connection) : connection_(connection) {}

PGResultSet PGQuery::execute(const std::string& sql) {
    if (!connection_.isConnected()) {
        throw ConnectionException("Cannot execute query: not connected to database.");
    }

    PGresult* res = PQexec(connection_.get(), sql.c_str());
    return processResult(res);
}

PGResultSet PGQuery::executeParams(const std::string& sql, const std::vector<std::string>& params) {
    if (!connection_.isConnected()) {
        throw ConnectionException("Cannot execute query: not connected to database.");
    }

    std::vector<const char*> paramPointers;
    for (const auto& p : params) {
        paramPointers.push_back(p.c_str());
    }

    PGresult* res = PQexecParams(
        connection_.get(), 
        sql.c_str(), 
        params.size(), 
        nullptr, // Let server infer types
        paramPointers.data(), 
        nullptr, // Default string parameters lengths
        nullptr, // Default string parameters formats
        0        // Text result format
    );

    return processResult(res);
}

PGResultSet PGQuery::processResult(PGresult* res) {
    ExecStatusType status = PQresultStatus(res);
    
    // Check for errors
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
        std::string errMessage = PQerrorMessage(connection_.get());
        PQclear(res);
        throw QueryException(errMessage);
    }

    PGResultSet resultSet;
    
    // Grab command status early (e.g. "UPDATE 5")
    if (char* cmdStatus = PQcmdStatus(res)) {
        resultSet.commandStatus = cmdStatus;
    }
    
    if (char* tuples = PQcmdTuples(res)) {
        if (*tuples != '\0') {
            try {
                resultSet.affectedRows = std::stoi(tuples);
            } catch (...) {
                // Ignore parse error on non-numeric tuples
            }
        }
    }

    // Process actual tuples if any
    if (status == PGRES_TUPLES_OK) {
        int nFields = PQnfields(res);
        int nTuples = PQntuples(res);

        for (int i = 0; i < nFields; ++i) {
            resultSet.headers.push_back(PQfname(res, i));
        }

        for (int i = 0; i < nTuples; ++i) {
            PGResultRow row;
            for (int j = 0; j < nFields; ++j) {
                if (PQgetisnull(res, i, j)) {
                    row.columns.push_back(std::nullopt);
                } else {
                    row.columns.push_back(std::string(PQgetvalue(res, i, j)));
                }
            }
            resultSet.rows.push_back(std::move(row));
        }
    }

    PQclear(res);
    return resultSet;
}

} // namespace pgcli::db
