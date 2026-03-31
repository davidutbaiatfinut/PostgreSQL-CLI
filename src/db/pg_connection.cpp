#include "pg_connection.hpp"
#include "db_exceptions.hpp"

namespace pgcli::db {

PGConnection::~PGConnection() {
    disconnect();
}

PGConnection::PGConnection(PGConnection&& other) noexcept : conn_(other.conn_) {
    other.conn_ = nullptr;
}

PGConnection& PGConnection::operator=(PGConnection&& other) noexcept {
    if (this != &other) {
        disconnect();
        conn_ = other.conn_;
        other.conn_ = nullptr;
    }
    return *this;
}

void PGConnection::connect(const std::string& connString) {
    disconnect(); // Ensure we don't leak anything if re-connecting

    conn_ = PQconnectdb(connString.c_str());

    if (PQstatus(conn_) != CONNECTION_OK) {
        std::string errMessage = PQerrorMessage(conn_);
        disconnect(); // Free the struct memory even on failure
        throw ConnectionException(errMessage);
    }
}

void PGConnection::disconnect() {
    if (conn_) {
        PQfinish(conn_);
        conn_ = nullptr;
    }
}

bool PGConnection::isConnected() const {
    if (!conn_) return false;
    return PQstatus(conn_) == CONNECTION_OK;
}

} // namespace pgcli::db
