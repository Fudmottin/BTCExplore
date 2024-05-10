#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "configfile.h"
#include "btcclient.h"

namespace asio = boost::asio;
using boost::system::error_code;
using asio::ip::tcp;

void handle_request(tcp::socket& socket, const ConfigFile& config) {
    asio::streambuf request;
    asio::read_until(socket, request, "\r\n\r\n");
    BTCClient client(config.rpc_url, std::to_string(config.rpc_port), config.auth);

    auto data = client.make_request("getblockchaininfo");

    // Construct the HTTP response with the JSON data
    std::string response =          
        "HTTP/1.1 200 OK\r\n"       
        "Content-Length: " + std::to_string(data.dump().length()) + "\r\n"    
        "Content-Type: application/json\r\n\r\n" +
        data.dump() + "\r\n";        

    asio::write(socket, asio::buffer(response));
}

void start_server(asio::io_context& io_context, const ConfigFile& config) {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), config.port));

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        auto future = std::async(std::launch::async, [socket = std::move(socket),
                                                      config = config]() mutable {
                                 handle_request(socket, config);
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
    std::cerr << "rpc_port: " << configFile.rpc_port << std::endl;
    std::cerr << "cookie_file_path: " << configFile.cookie_file_path << std::endl;
    std::cerr << "port: " << configFile.port << std::endl;

    // Start the server using io_context
    start_server(io_context, configFile);

    // Run the io_context
    io_context.run();

    return 0;
}
