#pragma once
#include "core/app_context.hpp"
#include "core/command_handler.hpp"
#include <string>
#include <vector>

namespace pgcli::cli {

class CLIParser {
public:
    explicit CLIParser(core::AppContext& context);
    
    // Enters an interactive read-eval-print loop
    void runInteractiveLoop();
    
    // Process a single line of input
    void processLine(const std::string& line);

private:
    core::AppContext& context_;
    core::CommandHandler commandHandler_;

    std::vector<std::string> splitArgs(const std::string& line);
};

} // namespace pgcli::cli
