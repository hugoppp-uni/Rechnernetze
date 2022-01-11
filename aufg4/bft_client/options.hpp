#include <cxxopts.hpp>
#include <iostream>

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