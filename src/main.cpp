#include <iostream>
#include <string>
#include <fstream>
#include <future>
#include <boost/beast.hpp>
#include "configfile.h"
#include "btcclient.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void handle_request(tcp::socket& socket, const ConfigFile& config) {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    http::read(socket, buffer, req);

    BTCClient client(config.rpc_url, std::to_string(config.rpc_port), config.auth);

    std::vector<std::pair<std::string, nlohmann::json>> requests = {
        {"getblockchaininfo", nullptr},
        {"getmempoolinfo", nullptr},
        {"getnetworkinfo", nullptr}
    };

    auto data = client.make_request(requests);

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, "BTCExplore");
    res.set(http::field::content_type, "application/json");
    res.body() = data.dump();
    res.prepare_payload();

    http::write(socket, res);
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
