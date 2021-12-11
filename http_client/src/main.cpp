#include <iostream>
#include <thread>
#include <atomic>
#include "logger.hpp"

#include "options.hpp"
#include "connection_listener.hpp"

void handle_incoming_requests(std::unique_ptr<Connection> cnn);

void remove_handler_thread(std::thread::id id);

std::list<std::thread> running_handler_threads{};
std::mutex handler_threads_mutex{};

std::atomic<bool> remove_handler_threads_on_completion{true};

/*
 * For now, you can test this with netcat:
 * 1. http_server /home -p 8080
 * 2. echo -n "number 1" | netcat -N localhost 8080 & echo -n "number 2" | netcat -N localhost 8080
 */
int main(int argc, char **argv) {

    Options opt{argc, argv};

    Logger::set_logfile(opt.logfile.has_value() ? opt.logfile.value() : "");
    Logger::set_log_to_console(!opt.logfile.has_value());
    Logger::set_level(Logger::level::WARN);

    if (!opt.port.has_value())
        opt.port = 8080;

    ConnectionListener listener = ConnectionListener{opt.port.value()};
    std::thread listener_thread{[&listener] {

        bool running = true;
        while (running) {
            try {
                handle_incoming_requests(listener.accept_next_connection());
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

    listener.close();
    listener_thread.join();

    remove_handler_threads_on_completion = false;
    if (!running_handler_threads.empty())
        Logger::warn(
            "Waiting for " + std::to_string(running_handler_threads.size()) + " active requests to complete...");

    int i{1};
    auto max_remaining_threads{std::to_string(running_handler_threads.size())};
    for (auto &handler_thread: running_handler_threads) {
        if (handler_thread.joinable())
            handler_thread.join();
        Logger::warn("... " + std::to_string(i++) + "/" + max_remaining_threads + " completed");
    }

    return 0;
}

void handle_incoming_requests(std::unique_ptr<Connection> cnn) {
    if (cnn) {
        running_handler_threads.emplace_back(std::thread{[cnn = std::move(cnn)] {

            Logger::info("Client connected from '" + cnn->get_address()->str() + "', receiving data");
            Logger::data(cnn->receive_string());
            std::this_thread::sleep_for(std::chrono::seconds(4));

            if (remove_handler_threads_on_completion) {
                remove_handler_thread(std::this_thread::get_id());
            }
        }});
    } else {
        Logger::error("Error while accepting client connection");
    }
}

void remove_handler_thread(std::thread::id id) {
    std::lock_guard<std::mutex> lock(handler_threads_mutex);
    auto iter = std::find_if(running_handler_threads.begin(), running_handler_threads.end(),
                             [id](std::thread &t) { return (t.get_id() == id); });
    if (iter != running_handler_threads.end()) {
        iter->detach();
        running_handler_threads.erase(iter);
    }
}
