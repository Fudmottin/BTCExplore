#ifndef BTCSERVER_H
#define BTCSERVER_H

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

namespace net = boost::asio;
using tcp = net::ip::tcp;

class BTCServer {
public:
    BTCServer() = delete;
    BTCServer(tcp::socket socket) : socket_(std::move(socket)) {};
    nlohmann::json read_request();
    void write_response(const nlohmann::json& data);

private:
    tcp::socket socket_;
};

#endif //BTCSERVER_H
