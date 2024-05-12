#include "btcserver.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using nlohmann::json;

nlohmann::json BTCServer::read_request() {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    http::read(socket_, buffer, req);

    // Dump request information
    std::cerr << "Request Method: " << req.method_string() << std::endl;
    std::cerr << "Request Target: " << req.target() << std::endl;
    for (const auto& header : req) {
        std::cerr << "Header: " << header.name_string() << " = " << header.value() << std::endl;
    }

    return nlohmann::json();
}

void BTCServer::write_response(const nlohmann::json& data) {
    http::response<http::string_body> res{http::status::ok, 11};
    res.set(http::field::server, "BTCExplore");
    res.set(http::field::content_type, "application/json");
    res.body() = data.dump();
    res.prepare_payload();

    http::write(socket_, res);
}

