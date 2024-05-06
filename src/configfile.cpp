#include "configfile.h"
#include <fstream>
#include <sstream>

ConfigFile::ConfigFile() {
    // Default constructor sets the path to the config file as the current working directory
    std::string filePath = "./BTCExplore.conf";
    loadConfigFile(filePath);
}

ConfigFile::ConfigFile(const std::string& filePath) {
    loadConfigFile(filePath);
}

void ConfigFile::loadConfigFile(const std::string& filePath) {
    std::ifstream configFile(filePath);
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                if (key == "rpcurl") {
                    rpc_url = value;
                } else if (key == "cookie") {
                    cookie_file_path = value;
                }
            }
        }
        configFile.close();
    } else {
        // Handle file open error
    }
}

