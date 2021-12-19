#include <cxxopts.hpp>
#include <iostream>
#include <filesystem>

class Options {
public:
    std::optional<int> port;
    std::optional<std::filesystem::path> logfile;
    std::filesystem::path document_root_folder;

    inline static std::string program_version = "0.1";
    inline const static std::string program_name = "http_server";
    int sleep_after_send;

    Options(int argc, char **argv) {
        cxxopts::Options options(program_name, "A simple HTTP/1.1 web server.");

        std::string slow_string;
        options.add_options()

            ("p,port", "Specify listening port", cxxopts::value(port))
            ("l,logfile", "Logfile to write to instead of stdout", cxxopts::value(logfile))
            ("S,sleep", "Sleep x seconds after sending the data", cxxopts::value(sleep_after_send))

            //info
            ("h,help", "Give this help list")
            ("usage", "Give a short usage message")
            ("V,version", "Print program version")

            //positional
            ("document_root_folder", "document_root_folder", cxxopts::value(document_root_folder));
        options.positional_help("DOCUMENT_ROOT_FOLDER");

        cxxopts::ParseResult result;
        try {
            options.parse_positional({"document_root_folder"});
            result = options.parse(argc, argv);
        } catch (cxxopts::OptionException& e) {
            std::cout << e.what() << std::endl;
            exit(1);
        }

        handle_info_params_and_exit_if_needed(options, result);
        validate_and_exit_if_needed(result);
    }

private:

    void validate_and_exit_if_needed(const cxxopts::ParseResult &result) const {
        if (port > 65535) {
            std::cout << "Port " << port.value() << " is invalid" << std::endl;
            exit(1);
        }

        if (document_root_folder.empty()) {
            std::cout << "No DOCUMENT_ROOT_FOLDER provided" << std::endl;
            exit(1);
        }

        if (!std::filesystem::exists(document_root_folder)) {
            std::cout << "DOCUMENT_ROOT_FOLDER " << document_root_folder << " does not exist";
            exit(1);
        }

        if (logfile.has_value() && !std::filesystem::exists(logfile.value().parent_path())) {
            std::cout << "logfile directory " << logfile.value().parent_path() << " does not exist";
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
            std::cout << program_version << std::endl;
            exit(0);
        }
        if (result.count("usage")) {
            std::cout << "TODO" << std::endl;
            exit(0);
        }
    }

};
