#include "cli_parser.hpp"
#include <iostream>
#include <sstream>
#include <iterator>

namespace pgcli::cli {

CLIParser::CLIParser(core::AppContext& context) 
    : context_(context), commandHandler_(context) {}

void CLIParser::runInteractiveLoop() {
    std::string line;
    std::cout << "pgcli - PostgreSQL CLI Manager for macOS\n";
    std::cout << "Type 'help' for available commands.\n\n";

    while (true) {
        std::cout << "pgcli> ";
        if (!std::getline(std::cin, line)) {
            break; // EOF
        }

        if (line.empty()) continue;

        // Disconnect automatically handled by AppContext destruction
        if (line == "exit" || line == "quit" || line == "\\q") {
            break;
        }

        processLine(line);
    }
}

void CLIParser::processLine(const std::string& line) {
    auto tokens = splitArgs(line);
    if (tokens.empty()) return;

    std::string command = tokens[0];
    tokens.erase(tokens.begin());

    commandHandler_.handleCommand(command, tokens);
}

std::vector<std::string> CLIParser::splitArgs(const std::string& line) {
    // Basic whitespace splitting for now
    std::istringstream iss(line);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());
    return results;
}

} // namespace pgcli::cli
