#pragma once

#include <httplib.h>
#include <nlohmann/json.hpp>

class BTCClient {
public:
    BTCClient() = delete;
    BTCClient(const std::string& url, const std::string& port, const std::string& auth);
    nlohmann::json make_request(const std::string& method, const nlohmann::json& params = nullptr);

private:
    std::string url_;
    std::string port_;
    std::string auth_;
};

