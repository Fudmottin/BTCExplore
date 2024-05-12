#include "btcserver.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <map>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using nlohmann::json;

static std::map<std::string, std::string> contentTypes = {
    {"json", "application/json"},
    {"html", "text/html"},
    {"css", "text/css"},
    {"js", "application/javascript"},
    {"jpg", "image/jpeg"},
    // Add more mappings as needed
};

static const std::string server_name = "BTC Explorer";

http::request<http::string_body> BTCServer::read_request() {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;

    http::read(socket_, buffer, req);

    // Dump request information
    std::cerr << "Request Method: " << req.method_string() << std::endl;
    std::cerr << "Request Target: " << req.target() << std::endl;
    for (const auto& header : req) {
        std::cerr << "Header: " << header.name_string() << " = " << header.value() << std::endl;
    }

    return req;
}

void BTCServer::write_response(const std::string& file_type, const std::string& data) {
    http::response<http::string_body> res{http::status::ok, 11};
    res.set(http::field::server, server_name);
    res.set(http::field::content_type, contentTypes[file_type]);
    res.body() = data;
    res.prepare_payload();

    http::write(socket_, res);
}

