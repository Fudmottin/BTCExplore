#include <iostream>
#include <string>
#include <fstream>
#include <future>
#include <boost/beast/core.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include "configfile.h"
#include "btcclient.h"
#include "btcserver.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace websocket = beast::websocket;
using tcp = net::ip::tcp;

void handle_request(tcp::socket& socket, const ConfigFile& config) {
    try {
        BTCServer server(std::move(socket));
        BTCClient client(config.rpc_url, std::to_string(config.rpc_port), config.auth);

        auto req = server.read_request();

        // Dump request information
        std::cerr << "Request Method: " << req.method_string() << std::endl;
        std::cerr << "Request Target: " << req.target() << std::endl;
        for (const auto& header : req) {
            std::cerr << "Header: " << header.name_string() << " = " << header.value() << std::endl;
        }


        if (websocket::is_upgrade(req)) {
            auto ws = server.upgrade_to_websocket(req);

            nlohmann::json initial_data = client.make_request({{"getblockchaininfo", nullptr}});
            ws.text(true);
            ws.write(net::buffer(initial_data.dump()));

            while (true) {
                beast::flat_buffer ws_buffer;
                ws.read(ws_buffer);
                // Process incoming WebSocket messages
                // For example, you could handle specific WebSocket messages here
                // and send responses back to the client
            }
        } else {
            if (req.method() == http::verb::get && req.target() == "/") {
                std::vector<std::pair<std::string, nlohmann::json>> requests = {
                    {"getblockchaininfo", nullptr},
                    {"getmempoolinfo", nullptr},
                    {"getnetworkinfo", nullptr}
                };

                auto data = client.make_request(requests);
                server.write_response<json>(data.dump());
            } else {
                server.write_not_found();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        // Optionally send an error response to the client
    }
}

void start_server(net::io_context& io_context, const ConfigFile& config) {
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
