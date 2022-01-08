#include <cxxopts.hpp>
#include <iostream>

struct slow_info {
    int bytes;
    int timeout;
};

class Options {
public:
    std::string server_ip;
    unsigned short server_port;
    std::string file_path;
    int retransmission_timeout_ms;
    bool debug;

    Options(int argc, char **argv) {
        cxxopts::Options options("bft_client", "Basic File Transfer Protocol Client");

        options.add_options()

            ("r,retrans_timeout_ms", "Retransmission timeout (RTO) in ms",
             cxxopts::value(retransmission_timeout_ms)->default_value("2000"))
            ("d,debug", "Produce debug output to stderr",
             cxxopts::value(debug))

            //info
            ("h,help", "Give this help list")

            //positional
            ("server", "server", cxxopts::value(server_ip))
            ("port", "port", cxxopts::value(server_port))
            ("file", "file", cxxopts::value(file_path));

        options.positional_help("SERVER PORT FILE");

        options.parse_positional({"server", "port", "file"});
        auto result = options.parse(argc, argv);

        handle_info_params_and_exit_if_needed(options, result);
        validate_and_exit_if_needed(result);
    }

private:

    static slow_info parse_slow_info(const std::string &slow_string) {
        unsigned long comma_idx = slow_string.find(',');
        int bytes = std::stoi(slow_string.substr(0, comma_idx));
        int timeout = std::stoi(slow_string.substr(comma_idx + 1));
        return slow_info{.bytes= bytes, .timeout= timeout};
    }

    void validate_and_exit_if_needed(const cxxopts::ParseResult &result) const {
        if (false) {
            exit(1);
        }
    }

    static void
    handle_info_params_and_exit_if_needed(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }
    }
};
