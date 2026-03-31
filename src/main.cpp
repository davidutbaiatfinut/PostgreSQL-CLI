#include "core/app_context.hpp"
#include "cli/cli_parser.hpp"
#include "ui/gui_app.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <exception>
#include <string>

int main(int argc, char* argv[]) {
    try {
        // High-level App Context containing Di-containers/managers
        pgcli::core::AppContext context;
        
        // Setup default logger
        context.getLogger().setLogFile("pgcli.log");

        // Try to load any previously saved profiles
        try {
            context.getConfig().load("pgcli_config.json");
        } catch (const std::exception& e) {
            context.getLogger().warn(std::string("Could not load config: ") + e.what() + ". Starting fresh.");
        }

        bool useGui = false;
        std::string commandLine;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--gui") {
                useGui = true;
            } else {
                commandLine += arg;
                if (i < static_cast<int>(argc) - 1) commandLine += " ";
            }
        }
        if (useGui) {
            context.getLogger().info("Initializing GUI mode...");
            pgcli::ui::runGui(context);
        } else {
            // CLI Mode
            pgcli::cli::CLIParser parser(context);
            if (!commandLine.empty()) {
                // Execute non-interactive immediate command
                parser.processLine(commandLine);
            } else {
                // Interactive TTY mode
                parser.runInteractiveLoop();
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown catastrophic failure.\n";
        return 1;
    }

    return 0;
}
