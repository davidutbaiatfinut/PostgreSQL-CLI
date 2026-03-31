#pragma once
#include "utils/config.hpp"
#include "db/pg_connection.hpp"
#include "utils/logger.hpp"

namespace pgcli::core {

class AppContext {
public:
    AppContext() = default;
    ~AppContext() {
        if (dbConnection_.isConnected()) {
            dbConnection_.disconnect();
        }
    }

    utils::ConfigManager& getConfig() { return config_; }
    db::PGConnection& getDatabase() { return dbConnection_; }
    utils::Logger& getLogger() { return utils::Logger::getInstance(); }

private:
    utils::ConfigManager config_;
    db::PGConnection dbConnection_;
};

} // namespace pgcli::core
