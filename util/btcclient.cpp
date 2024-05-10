#include "btcclient.h"

using nlohmann::json;

BTCClient::BTCClient(const std::string& url, const std::string& port, const std::string& auth)
    : url_(url), port_(port), auth_(auth) {}

json BTCClient::make_request(const std::string& method, const json& params) {
    std::string full_url = url_ + ":" + port_;
    httplib::Client client(full_url.c_str());
    httplib::Headers headers = {
        {"Content-Type", "application/json"},
        {"Accept", "application/json"},
        {"Authorization", "Basic " + auth_}
    };

    json request_data = {
        {"jsonrpc", "1.0"},
        {"id", method},
        {"method", method},
        {"params", params}
    };

    std::string body = request_data.dump();
    auto res = client.Post("/", headers, body, "application/json");

    if (res && res->status == 200) {
        return json::parse(res->body);
    } else {
        std::cerr << "Request failed with status: " << res->status << std::endl;
        return nullptr;
    }
}
