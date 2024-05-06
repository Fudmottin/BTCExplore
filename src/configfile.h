#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>

class ConfigFile {
public:
    ConfigFile();
    explicit ConfigFile(const std::string& filePath);

    std::string rpc_url = "http://127.0.0.1:8332";
    std::string cookie_file_path;

private:
    void loadConfigFile(const std::string& filePath);
};

#endif // CONFIGFILE_H

