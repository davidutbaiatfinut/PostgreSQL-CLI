#pragma once
#include <string>
#include <libpq-fe.h>

namespace pgcli::db {

class PGConnection {
public:
    PGConnection() = default;
    ~PGConnection();

    // Prevent copying
    PGConnection(const PGConnection&) = delete;
    PGConnection& operator=(const PGConnection&) = delete;

    // Allow moving
    PGConnection(PGConnection&& other) noexcept;
    PGConnection& operator=(PGConnection&& other) noexcept;

    // Connecting to database
    void connect(const std::string& connString);
    void disconnect();

    // Status
    bool isConnected() const;
    
    // Low-level access for queries
    PGconn* get() const { return conn_; }

private:
    PGconn* conn_{nullptr};
};

} // namespace pgcli::db
