#include "config.hpp"
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pgcli::utils {

ConfigManager::ConfigManager() {}

void ConfigManager::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Just return, empty starting state is fine if file doesn't exist
        return;
    }

    try {
        json j;
        file >> j;

        if (j.contains("active_profile") && j["active_profile"].is_string()) {
            activeProfileName_ = j["active_profile"];
        }

        if (j.contains("profiles") && j["profiles"].is_array()) {
            for (const auto& profileJson : j["profiles"]) {
                ConnectionProfile profile;
                if (profileJson.contains("name")) profile.name = profileJson["name"];
                if (profileJson.contains("conn_string")) profile.connectionString = profileJson["conn_string"];
                if (!profile.name.empty()) {
                    profiles_.push_back(profile);
                }
            }
        }
    } catch (const json::parse_error& e) {
        throw std::runtime_error("Failed to parse config file: " + std::string(e.what()));
    }
}

void ConfigManager::save(const std::string& filepath) const {
    json j;
    j["active_profile"] = activeProfileName_;
    
    json profilesJson = json::array();
    for (const auto& profile : profiles_) {
        json p;
        p["name"] = profile.name;
        p["conn_string"] = profile.connectionString;
        profilesJson.push_back(p);
    }
    j["profiles"] = profilesJson;

    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file for writing");
    }
    
    file << j.dump(4);
}

void ConfigManager::addProfile(const ConnectionProfile& profile) {
    for (auto& p : profiles_) {
        if (p.name == profile.name) {
            p.connectionString = profile.connectionString;
            return; // Update existing
        }
    }
    profiles_.push_back(profile);
}

void ConfigManager::removeProfile(const std::string& name) {
    profiles_.erase(std::remove_if(profiles_.begin(), profiles_.end(),
        [&name](const ConnectionProfile& p) { return p.name == name; }), profiles_.end());
    
    if (activeProfileName_ == name) {
        activeProfileName_ = "";
    }
}

std::optional<ConnectionProfile> ConfigManager::getProfile(const std::string& name) const {
    for (const auto& p : profiles_) {
        if (p.name == name) {
            return p;
        }
    }
    return std::nullopt;
}

void ConfigManager::setActiveProfile(const std::string& name) {
    activeProfileName_ = name;
}

std::optional<ConnectionProfile> ConfigManager::getActiveProfile() const {
    if (activeProfileName_.empty()) return std::nullopt;
    return getProfile(activeProfileName_);
}

std::vector<ConnectionProfile> ConfigManager::getAllProfiles() const {
    return profiles_;
}

} // namespace pgcli::utils
