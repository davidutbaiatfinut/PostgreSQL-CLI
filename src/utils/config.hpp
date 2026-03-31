#pragma once
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>

namespace pgcli::utils {

struct ConnectionProfile {
    std::string name;
    std::string connectionString;
};

class ConfigManager {
public:
    ConfigManager();
    
    // Throws std::runtime_error on parse failure
    void load(const std::string& filepath);
    void save(const std::string& filepath) const;

    void addProfile(const ConnectionProfile& profile);
    void removeProfile(const std::string& name);
    
    std::optional<ConnectionProfile> getProfile(const std::string& name) const;
    
    void setActiveProfile(const std::string& name);
    std::optional<ConnectionProfile> getActiveProfile() const;
    
    std::vector<ConnectionProfile> getAllProfiles() const;

private:
    std::vector<ConnectionProfile> profiles_;
    std::string activeProfileName_;
};

} // namespace pgcli::utils
