#include <cxxopts.hpp>
#include <iostream>
#include <filesystem>

class Options {
public:
    unsigned short port;
    std::string directory;
    bool debug;

    Options(int argc, char **argv) {
        cxxopts::Options options("bft_server", "Basic File Transfer Protocol Server");

        options.add_options()

            ("d,debug", "Produce debug output to stderr",
             cxxopts::value(debug))

            //info
            ("h,help", "Give this help list")

            //positional
            ("port", "port", cxxopts::value(port))
            ("directory", "directory", cxxopts::value(directory));

        options.positional_help("PORT DIRECTORY");

        options.parse_positional({"port", "directory"});
        auto result = options.parse(argc, argv);

        handle_info_params_and_exit_if_needed(options, result);
        validate_and_exit_if_needed(result);
    }

private:

    void validate_and_exit_if_needed(const cxxopts::ParseResult &result) const {
        const std::filesystem::path &dir = std::filesystem::path(directory);
        if (!exists(dir)) {
            create_directories(dir);
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
