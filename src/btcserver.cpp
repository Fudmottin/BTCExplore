#include "btcserver.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

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

