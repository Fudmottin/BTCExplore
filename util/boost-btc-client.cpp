#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "../external/cppcodec/cppcodec/base64_default_rfc4648.hpp"

using json = nlohmann::json;
using boost::asio::ip::tcp;

std::string read_cookie_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error opening cookie file: " << filepath << std::endl;
        return "";
    }
    std::string line;
    std::getline(file, line);
    return cppcodec::base64_rfc4648::encode(line);
}

class JsonRequest {
public:
    JsonRequest(boost::asio::io_context& io_context, const std::string& url, const std::string& auth)
    : resolver_(io_context), socket_(io_context), url_(url), auth_(auth) {}

    json make_request(const std::string& method, const json& params = nullptr) {
        tcp::resolver::results_type endpoints = resolver_.resolve(url_, "8332");
        boost::asio::connect(socket_, endpoints);

        json request_data = {
            {"jsonrpc", "1.0"},
            {"id", method},
            {"method", method},
            {"params", params}
        };

        std::stringstream request_stream;
        request_stream << "POST / HTTP/1.1\r\n";
        request_stream << "Host: " << url_ << "\r\n";
        request_stream << "Content-Type: application/json\r\n";
        request_stream << "Authorization: Basic " << auth_ << "\r\n";
        request_stream << "Content-Length: " << request_data.dump().length() << "\r\n\r\n";
        request_stream << request_data.dump();

        boost::asio::write(socket_, boost::asio::buffer(request_stream.str()));

        boost::asio::streambuf response;
        boost::asio::read_until(socket_, response, "\r\n");

        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);

        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            std::cerr << "Invalid response" << std::endl;
            return nullptr;
        }

        if (status_code != 200) {
            std::cerr << "Request failed with status: " << status_code << std::endl;
            return nullptr;
        }

        // Throw away the rest of the headers
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {}

        // Read the rest of the response
        std::stringstream response_body;
        boost::asio::read(socket_, response, boost::asio::transfer_at_least(1));
        response_body << &response;

        return json::parse(response_body.str());
    }

private:
    tcp::resolver resolver_;
    tcp::socket socket_;
    std::string url_;
    std::string auth_;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <RPC method> [param1] [param2] ..." << std::endl;
        return 1;
    }

    boost::asio::io_context io_context;
    std::string rpc_url = "127.0.0.1";
    std::string cookie_file_path = "/Volumes/Bitcoin/blockchain/.cookie";  // Replace with the path to your .cookie file
    std::string auth = read_cookie_file(cookie_file_path);

    if (auth.empty()) {
        std::cerr << "Error reading cookie file" << std::endl;
        return 1;
    }

    JsonRequest request(io_context, rpc_url, auth);
    std::string method(argv[1]);

    json params = json::array();
    for (int i = 2; i < argc; ++i) {
        params.push_back(argv[i]);
    }

    json response = request.make_request(method, params);

    if (!response.empty()) {
        std::cout << "Response: " << response.dump(4) << std::endl;
    }

    return 0;
}

