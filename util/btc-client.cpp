#include <iostream>
#include <fstream>
#include <sstream>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <bitset>
#include <vector>

using namespace std;
using json = nlohmann::json;

string base64_encode(const string& input) {
    static const string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    string encoded;
    unsigned int val = 0;
    int bits = -6;

    for (const auto& c : input) {
        val = (val << 8) + c;
        bits += 8;

        while (bits >= 0) {
            encoded.push_back(base64_chars[(val >> bits) & 0x3F]);
            bits -= 6;
        }
    }

    if (bits > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (bits + 8)) & 0x3F]);
    }

    while (encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

string read_cookie_file(const string& filepath) {
    ifstream file(filepath);
    if (!file) {
        cerr << "Error opening cookie file: " << filepath << endl;
        return "";
    }
    string line;
    getline(file, line);
    return base64_encode(line);
}

class JsonRequest {
public:
    JsonRequest(const string& url, const string& auth)
        : url_(url), auth_(auth) {}

    json make_request(const string& method, const json& params = nullptr) {
        httplib::Client client(url_.c_str());
        httplib::Headers headers = {
            {"Content-Type", "application/json"},
            {"Accept", "application/json"},
            {"Authorization", "Basic " + auth_}
        };

        json request_data = {
            {"jsonrpc", "1.0"},
            {"id", method},
            {"method", method},
            {"params", params}
        };

        string body = request_data.dump();
        auto res = client.Post("/", headers, body, "application/json");

        if (res && res->status == 200) {
            return json::parse(res->body);
        } else {
            cerr << "Request failed with status: " << res->status << endl;
            return nullptr;
        }
    }

private:
    string url_;
    string auth_;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <RPC method> [param1] [param2] ..." << endl;
        return 1;
    }

    string rpc_url = "http://127.0.0.1:8332";
    string cookie_file_path = "/Volumes/Bitcoin/blockchain/.cookie";  // Replace with the path to your .cookie file
    string auth = read_cookie_file(cookie_file_path);

    if (auth.empty()) {
        cerr << "Error reading cookie file" << endl;
        return 1;
    }

    JsonRequest request(rpc_url, auth);
    string method(argv[1]);

    json params = json::array();
    for (int i = 2; i < argc; ++i) {
        params.push_back(argv[i]);
    }

    json response = request.make_request(method, params);

    if (!response.empty()) {
        cout << "Response: " << response.dump(4) << endl;
    }

    return 0;
}
