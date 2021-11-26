#include <cxxopts.hpp>
#include <iostream>

class Options {
public:
    std::string url;
    std::string output;
    std::string range;
    bool remote_name;
    bool slow{false}; // TODO: If slow option was set, BYTES and TIMEOUT must be parsed
    bool verbose{false};

    Options(int argc, char **argv) {
        cxxopts::Options options("snatch", "A simple HTTP/1.1 client.");

        options.add_options()

            ("o,output", "Output to FILE instead of stdout",
             cxxopts::value(output))
            ("O,remote-name", "Output to FILE, named like the remote document (extracted from URL), instead of stdout",
             cxxopts::value(remote_name))
            ("r,range", "Retrieve a byte range (i.e. partial document)",
             cxxopts::value(range))
            ("s,slow", "Transmit request in chunks of BYTES every N milliseconds (TIMEOUT) - emulates a typed request",
             cxxopts::value(slow))
            ("v,verbose", "Produce verbose output to stderr (request and response metadata)",
             cxxopts::value(verbose))

            //info
            ("h,help", "Give this help list")
            ("usage", "Give a short usage message")
            ("V,version", "Print program version")

            //positional
            ("url", "url", cxxopts::value(url));

        options.positional_help("URL");

        options.parse_positional({"url"});
        auto result = options.parse(argc, argv);

        handle_info_params_and_exit_if_needed(options, result);
        validate_and_exit_if_needed(result);
    }

private:

    void validate_and_exit_if_needed(const cxxopts::ParseResult &result) const {
        if (!range.empty() && !std::regex_match(range, std::regex("\\d*-(?:\\d*)?"))) {
            std::cout << "Range needs to 2 items long" << std::endl;
            exit(1);
        }
    }

    static void
    handle_info_params_and_exit_if_needed(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }
        if (result.count("version")) {
            std::cout << "v0.1" << std::endl;
            exit(0);
        }
        if (result.count("usage")) {
            std::cout << "TODO" << std::endl;
            exit(0);
        }
    }
};
