#ifndef BTCSERVER_H
#define BTCSERVER_H

#include <string>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>
#include "content_types.h"

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = net::ip::tcp;

class BTCServer {
public:
    BTCServer() = delete;
    BTCServer(tcp::socket socket) : socket_(std::move(socket)) {};

    tcp::socket& socket() { return socket_; }

    http::request<http::string_body> read_request() {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket_, buffer, req);
        return req;
    }

    template <typename FileType>
    void write_response(const std::string& data) {
        http::response<http::string_body> res{http::status::ok, 11};
        res.set(http::field::server, "BTC Explorer");
        res.set(http::field::content_type, content_type<FileType>::value);
        res.body() = data;
        res.prepare_payload();
        http::write(socket_, res);
    }

    void write_not_found() {
        http::response<http::string_body> res{http::status::not_found, 11};
        res.set(http::field::server, "BTC Explorer");
        res.set(http::field::content_type, "text/plain");
        res.body() = "Not Found";
        res.prepare_payload();
        http::write(socket_, res);
    }

    websocket::stream<tcp::socket> upgrade_to_websocket(const http::request<http::string_body>& req) {
        websocket::stream<tcp::socket> ws{std::move(socket_)};
        ws.accept(req);
        return ws;
    }

private:
    tcp::socket socket_;
};

#endif // BTCSERVER_H

