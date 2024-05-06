#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "configfile.h"

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
        "Hello, World!\r\n";

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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>\n";
        return 1;
    }

    boost::asio::io_context io_context;

    // Parse command line argument for config file path
    std::string configFilePath = argv[1];

    // Create ConfigFile object based on the provided path
    ConfigFile configFile(configFilePath);

    // Access properties of configFile as needed
    std::cerr << "rpc_url: " << configFile.rpc_url << std::endl;
    std::cerr << "cookie_file_path: " << configFile.cookie_file_path << std::endl;

    // Start the server using io_context
    start_server(io_context, 2024);

    // Run the io_context
    io_context.run();

    return 0;
}
