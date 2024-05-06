#include <fstream>
#include <sstream>
#include <iostream>
#include "configfile.h"

ConfigFile::ConfigFile() {}

ConfigFile::ConfigFile(const std::string& filePath) {
    loadConfigFile(filePath);
}

void ConfigFile::loadConfigFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss >> std::ws, value)) {
            // Trim leading/trailing whitespaces from key and value
            key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

            // Remove quotes from value if present
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }

            if (key == "rpcurl") {
                rpc_url = value;
            } else if (key == "cookie") {
                cookie_file_path = value;
            } else if (key == "port") {
                // Assuming port is an integer
                try {
                    port = std::stoi(value);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid port value: " << value << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Port value out of range: " << value << std::endl;
                }
            } else {
                std::cerr << "Unknown key: " << key << std::endl;
            }
        }
    }

    file.close();
}

