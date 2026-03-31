#include "command_handler.hpp"
#include "db/pg_query.hpp"
#include "db/db_exceptions.hpp"
#include "cli/formatter.hpp" // We'll implement this next
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>

namespace pgcli::core {

CommandHandler::CommandHandler(AppContext& context) : context_(context) {}

void CommandHandler::handleCommand(const std::string& command, const std::vector<std::string>& args) {
    try {
        if (command == "help" || command == "\\?") {
            printHelp();
        } else if (command == "connect" || command == "\\c") {
            executeConnect(args);
        } else if (command == "disconnect" || command == "\\q") {
            executeDisconnect();
        } else if (command == "profile") {
            executeProfile(args);
        } else if (command == "query" || command == "q") {
            executeQuery(args);
        } else if (command == "list_tables" || command == "\\dt") {
            executeListTables();
        } else if (command == "describe" || command == "\\d") {
            executeDescribe(args);
        } else if (command == "export") {
            executeExport(args);
        } else {
            context_.getLogger().warn("Unknown command: " + command + ". Type 'help' for usage.");
        }
    } catch (const db::DatabaseException& e) {
        context_.getLogger().error(e.what());
    } catch (const std::exception& e) {
        context_.getLogger().error("Unexpected error: " + std::string(e.what()));
    }
}

void CommandHandler::executeConnect(const std::vector<std::string>& args) {
    if (args.empty()) {
        context_.getLogger().warn("Usage: connect <connection_string> OR connect profile <profile_name>");
        return;
    }

    std::string connStr;
    if (args[0] == "profile" && args.size() > 1) {
        auto profile = context_.getConfig().getProfile(args[1]);
        if (!profile) {
            context_.getLogger().error("Profile not found: " + args[1]);
            return;
        }
        connStr = profile->connectionString;
        context_.getConfig().setActiveProfile(args[1]);
    } else {
        connStr = args[0];
    }

    context_.getLogger().info("Connecting to database...");
    context_.getDatabase().connect(connStr);
    context_.getLogger().info("Connected successfully.");
}

void CommandHandler::executeDisconnect() {
    context_.getDatabase().disconnect();
    context_.getLogger().info("Disconnected.");
}

void CommandHandler::executeQuery(const std::vector<std::string>& args) {
    if (args.empty()) {
        context_.getLogger().warn("Usage: query <SQL>");
        return;
    }

    // Join args into single sql string
    std::string sql;
    for (size_t i = 0; i < args.size(); ++i) {
        sql += args[i] + (i < args.size() - 1 ? " " : "");
    }

    db::PGQuery queryRunner(context_.getDatabase());
    db::PGResultSet result = queryRunner.execute(sql);

    // Print result using formatter
    cli::Formatter formatter;
    formatter.printTable(result);
}

void CommandHandler::executeListTables() {
    std::string sql = R"(
        SELECT table_schema, table_name 
        FROM information_schema.tables 
        WHERE table_schema NOT IN ('information_schema', 'pg_catalog') 
        ORDER BY table_schema, table_name;
    )";
    
    db::PGQuery queryRunner(context_.getDatabase());
    db::PGResultSet result = queryRunner.execute(sql);

    cli::Formatter formatter;
    formatter.printTable(result);
}

void CommandHandler::executeDescribe(const std::vector<std::string>& args) {
    if (args.empty()) {
        context_.getLogger().warn("Usage: describe <table>");
        return;
    }

    std::string table = args[0];
    std::string sql = "SELECT column_name, data_type, character_maximum_length "
                      "FROM information_schema.columns "
                      "WHERE table_name = $1;";
    
    db::PGQuery queryRunner(context_.getDatabase());
    db::PGResultSet result = queryRunner.executeParams(sql, {table});

    cli::Formatter formatter;
    formatter.printTable(result);
}

void CommandHandler::executeExport(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        context_.getLogger().warn("Usage: export <table> <filename.csv>");
        return;
    }
    
    std::string table = args[0];
    std::string filename = args[1];
    
    // We do a simple select *
    std::string sql = "SELECT * FROM " + table + ";"; // Watch out for SQL injection in a real app, though this is a CLI admin tool.
    
    db::PGQuery queryRunner(context_.getDatabase());
    db::PGResultSet result = queryRunner.execute(sql);
    
    cli::Formatter formatter;
    formatter.exportToCSV(result, filename);
    context_.getLogger().info("Exported to " + filename);
}

void CommandHandler::executeProfile(const std::vector<std::string>& args) {
    if (args.empty() || args[0] == "list") {
        auto profiles = context_.getConfig().getAllProfiles();
        std::cout << "Saved profiles:\n";
        for (const auto& p : profiles) {
            std::cout << " - " << p.name << "\n";
        }
    } else if (args[0] == "add" && args.size() >= 3) {
        utils::ConnectionProfile p{args[1], args[2]};
        context_.getConfig().addProfile(p);
        context_.getConfig().save("config.json"); // Hardcoded path just for demo
        context_.getLogger().info("Profile saved.");
    }
}

void CommandHandler::printHelp() {
    std::cout << "Available commands:\n"
              << "  help / \\?                  - Show this help message\n"
              << "  connect <conn_string>      - Connect to database\n"
              << "  connect profile <name>     - Connect using saved profile\n"
              << "  disconnect / \\q            - Disconnect from database\n"
              << "  profile list               - List saved connection profiles\n"
              << "  profile add <name> <conn>  - Save a connection profile\n"
              << "  query <SQL> / q <SQL>      - Execute arbitrary SQL\n"
              << "  list_tables / \\dt         - List all tables in current database\n"
              << "  describe <table> / \\d    - Show table schema\n"
              << "  export <table> <file>      - Export table data to CSV\n"
              << "  exit / quit                - Exit the application\n";
}

} // namespace pgcli::core
