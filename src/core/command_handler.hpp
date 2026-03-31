#pragma once
#include "app_context.hpp"
#include <string>
#include <vector>

namespace pgcli::core {

class CommandHandler {
public:
    explicit CommandHandler(AppContext& context);

    // Entry point for executing a parsed command
    void handleCommand(const std::string& command, const std::vector<std::string>& args);

private:
    AppContext& context_;

    void executeConnect(const std::vector<std::string>& args);
    void executeDisconnect();
    
    // Core database commands
    void executeQuery(const std::vector<std::string>& args);
    void executeListTables();
    void executeDescribe(const std::vector<std::string>& args);
    void executeExport(const std::vector<std::string>& args);
    
    // Profile commands
    void executeProfile(const std::vector<std::string>& args);
    
    void printHelp();
};

} // namespace pgcli::core
