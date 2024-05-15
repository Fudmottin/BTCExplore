#ifndef BTCSERVER_H
#define BTCSERVER_H

#include <string>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include "content_types.h"

namespace net = boost::asio;
namespace http = boost::beast::http;
using tcp = net::ip::tcp;

class BTCServer {
public:
    BTCServer() = delete;
    BTCServer(tcp::socket socket) : socket_(std::move(socket)) {};
    http::request<http::string_body> read_request();

    template <typename FileType>
    void write_response(const std::string& data) {
        http::response<http::string_body> res{http::status::ok, 11};
        res.set(http::field::server, "BTC Explorer");
        res.set(http::field::content_type, content_type<FileType>::value);
        res.body() = data;
        res.prepare_payload();

        http::write(socket_, res);
    }

private:
    tcp::socket socket_;
};

#endif //BTCSERVER_H
