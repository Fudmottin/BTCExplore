#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace asio = boost::asio;
using boost::system::error_code;
using asio::ip::tcp;

void handle_request(tcp::socket& socket) {
    asio::streambuf request;
    asio::read_until(socket, request, "\r\n\r\n");

    // Respond with a simple HTTP message
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 13\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Hello, World!";

    asio::write(socket, asio::buffer(response));
}

void start_server(asio::io_context& io_context, unsigned short port) {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::async(std::launch::async, [socket = std::move(socket)]() mutable {
            handle_request(socket);
        });
    }
}

int main() {
    asio::io_context io_context;

    start_server(io_context, 2024);

    io_context.run();

    return 0;
}

