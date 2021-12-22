#include <iostream>
#include <thread>
#include <atomic>
#include "logger.hpp"

#include "options.hpp"
#include "connection_listener.hpp"
#include "response_factory.h"
#include <fmt/format.h>

void remove_handler_thread(std::thread::id id);

void handle_incoming_request(std::unique_ptr<Connection> cnn, const Options &opt);
void handle_incoming_request_no_expt(std::unique_ptr<Connection> cnn, const Options &opt);

std::list<std::thread> running_handler_threads{};
std::mutex handler_threads_mutex{};

bool remove_handler_threads_on_completion{true};


/*
 * For now, you can test this with netcat:
 * 1. http_server /home -p 8080
 * 2. echo -n "number 1" | netcat -N localhost 8080 & echo -n "number 2" | netcat -N localhost 8080
 */
int main(int argc, char **argv) {

    Options opt{argc, argv};

    Logger::set_logfile(opt.logfile.has_value() ? opt.logfile.value() : "");
    Logger::set_log_to_console(!opt.logfile.has_value());
    Logger::set_level(Logger::level::DATA);

    if (!opt.port.has_value())
        opt.port = 8080;

    ConnectionListener listener = ConnectionListener{opt.port.value()};
    std::thread listener_thread{[&listener, &opt] {

        bool running = true;
        while (running) {
            try {
                std::unique_ptr<Connection> cnn = listener.accept_next_connection();
                if (cnn) {
                    std::lock_guard<std::mutex> lock{handler_threads_mutex};
                    running_handler_threads.emplace_back(
                        std::thread(handle_incoming_request_no_expt, std::move(cnn), std::ref(opt))
                    );
                } else {
                    Logger::error("Error while accepting client connection");
                }
            } catch (ListenerClosedException &) {
                running = false;
            }
        }

    }};

    int c{0};
    while (c != 'q') {
        c = getchar();
    }
    std::cout << std::endl;

    listener.shutdown();
    listener_thread.join();

    {
        std::lock_guard<std::mutex> lock{handler_threads_mutex};
        remove_handler_threads_on_completion = false;
    }

    if (!running_handler_threads.empty())
        Logger::warn(fmt::format("Waiting for {} active requests to complete...", running_handler_threads.size()));

    int i{1};
    auto max_remaining_threads{std::to_string(running_handler_threads.size())};
    for (auto &handler_thread: running_handler_threads) {
        if (handler_thread.joinable())
            handler_thread.join();
        Logger::warn(fmt::format("... {}/{} completed", i, max_remaining_threads));
    }

    return 0;
}


void remove_handler_thread(std::thread::id id) {
    auto iter = std::find_if(running_handler_threads.begin(), running_handler_threads.end(),
                             [id](std::thread &t) { return (t.get_id() == id); });
    if (iter != running_handler_threads.end()) {
        iter->detach();
        running_handler_threads.erase(iter);
    }
}

void handle_incoming_request(std::unique_ptr<Connection> cnn, const Options &opt) {
    const std::string &peer_address = cnn->get_address()->str();
    Logger::info(fmt::format("Client connected from '{}', receiving data", peer_address));
    std::string received = cnn->receive_string();

    // Build request out of received data and handle it
    std::unique_ptr<HttpRequest> request{nullptr};
    try{
        request = std::make_unique<HttpRequest>(received);
    }
    catch(...){
        std::string err_str{"Could not parse request"};
        Logger::warn(err_str);
        auto response = ResponseFactory::create_from_plain_text(HttpResponse::Status::BAD_REQUEST, err_str);
        cnn->send(response);
        return;
    }

    Logger::data(fmt::format("[{}] requested {}", peer_address, request->get_uri()));

    std::string log{};
    HttpResponse response = ResponseFactory::create(*request, opt.document_root_folder, log);
    const std::string &string = fmt::format("[{address}] responding with '{code} {text}' '{log}'",
                                            fmt::arg("address", peer_address),
                                            fmt::arg("code", response.get_status_code()),
                                            fmt::arg("text", response.get_status_text()),
                                            fmt::arg("log", log));
    Logger::data(string);
    cnn->send(response);

    if (opt.sleep_after_send > 0) {
        Logger::warn(fmt::format("sleeping for {} seconds", opt.sleep_after_send));
        std::this_thread::sleep_for(std::chrono::seconds(opt.sleep_after_send));
    }

    std::lock_guard<std::mutex> lock{handler_threads_mutex};
    if (remove_handler_threads_on_completion) {
        remove_handler_thread(std::this_thread::get_id());
    }
}

void handle_incoming_request_no_expt(std::unique_ptr<Connection> cnn, const Options &opt) {
    try {
        handle_incoming_request(std::move(cnn), opt);
    }
    catch (...){
        Logger::error("exception occurred while handling request");
    }
}
