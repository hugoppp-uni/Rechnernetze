#include <cxxopts.hpp>
#include <iostream>

class Options {
public:
    std::string output;
    std::string remote_name;
    std::vector<int> range;
    bool slow{false};
    bool verbose{false};

    Options(int argc, char **argv) : output() {
        cxxopts::Options options("snatch", "A simple  HTTP /1.1  client.");
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

            ("h,help", "Give this help list")
            ("usage", "Give a short usage message")
            ("V,version", "Print program version");

        auto result = options.parse(argc, argv);
        HandleInfoParamsAndExitIfNeeded(options, result);
        ValidateAndExitIfNeeded(result);

    }

private:

    void ValidateAndExitIfNeeded(const cxxopts::ParseResult &result) const {
        if (range.size() != 2) {
            std::cout << "Range needs to 2 items long" << std::endl;
            exit(1);
        }
    }

    static void HandleInfoParamsAndExitIfNeeded(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
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
