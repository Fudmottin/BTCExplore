#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "btcclient.h"
#include "../external/cppcodec/cppcodec/base64_default_rfc4648.hpp"

std::string read_cookie_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error opening cookie file: " << filepath << std::endl;
        return "";
    }
    std::string line;
    std::getline(file, line);
    return cppcodec::base64_rfc4648::encode(line);
}

int main(int argc, char* argv[]) {
    using nlohmann::json;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <RPC method> [param1] [param2] ..." << std::endl;
        return 1;
    }

    std::string rpc_url = "127.0.0.1";
    std::string rpc_port = "8332";
    std::string cookie_file_path = "/Volumes/Bitcoin/blockchain/.cookie";  // Replace with the path to your .cookie file
    std::string auth = read_cookie_file(cookie_file_path);

    if (auth.empty()) {
        std::cerr << "Error reading cookie file" << std::endl;
        return 1;
    }

    BTCClient request(rpc_url, rpc_port, auth);
    std::string method(argv[1]);

    json params = json::array();
    for (int i = 2; i < argc; ++i) {
        params.push_back(argv[i]);
    }

    auto response = request.make_request(method, params);

    if (!response.empty()) {
        std::cout << "Response: " << response.dump() << std::endl;
    }

    return 0;
}

