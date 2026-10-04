#include "UCI.hpp"
#include "Search.hpp"

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

namespace UCI {

std::string move_to_string(const Move& m) {
    int from = m.from_square();
    int to = m.to_square();

    std::string s = "";
    s += static_cast<char>('a' + (from % 8));
    s += static_cast<char>('1' + (from / 8));
    s += static_cast<char>('a' + (to % 8));
    s += static_cast<char>('1' + (to / 8));

    // Append promotion piece character if applicable
    if (m.is_promotion()) {
        s += m.promotion_char(); // 'q', 'r', 'b', 'n'
    }
    return s;
}

Move parse_move(Position& pos, const std::string& move_str) {
    std::vector<Move> legal_moves;
    pos.generate_legal_moves(legal_moves);

    for (const auto& m : legal_moves) {
        if (move_to_string(m) == move_str) {
            return m;
        }
    }
    return Move::none(); // Return invalid / null move if not matched
}

void loop() {
    Position pos;
    std::string line;
    std::thread search_thread;
    std::atomic_bool stop_search{false};
    std::mutex output_mutex;

    auto finish_search = [&]() {
        if (search_thread.joinable()) {
            stop_search.store(true, std::memory_order_relaxed);
            search_thread.join();
        }
    };

    // Disable C++ I/O synchronization with C stdio for faster, predictable flushing
    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "uci") {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "id name Blunder Engine\n";
            std::cout << "id author Blunder Engine contributors\n";
            std::cout << "uciok\n" << std::flush;
        }
        else if (command == "isready") {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "readyok\n" << std::flush;
        }
        else if (command == "ucinewgame") {
            finish_search();
            stop_search.store(false, std::memory_order_relaxed);
            pos.set_startpos();
        }
        else if (command == "position") {
            finish_search();
            stop_search.store(false, std::memory_order_relaxed);
            std::string type;
            iss >> type;

            if (type == "startpos") {
                pos.set_startpos();
                std::string moves_token;
                if (iss >> moves_token && moves_token == "moves") {
                    std::string move_token;
                    while (iss >> move_token) {
                        Move m = parse_move(pos, move_token);
                        if (m.is_valid()) pos.make_move(m);
                    }
                }
            } 
            else if (type == "fen") {
                std::string fen;
                for (int i = 0; i < 6; ++i) {
                    std::string part;
                    if (iss >> part) fen += part + " ";
                }
                pos.set_from_fen(fen);

                std::string moves_token;
                if (iss >> moves_token && moves_token == "moves") {
                    std::string move_token;
                    while (iss >> move_token) {
                        Move m = parse_move(pos, move_token);
                        if (m.is_valid()) pos.make_move(m);
                    }
                }
            }
        }
        else if (command == "go") {
            finish_search();
            stop_search.store(false, std::memory_order_relaxed);
            Search::Limits limits;
            int white_time = -1;
            int black_time = -1;
            int white_increment = 0;
            int black_increment = 0;
            int moves_to_go = 30;
            bool infinite = false;
            bool depth_limited = false;
            std::string token;
            while (iss >> token) {
                int value = 0;
                if (token == "depth" && iss >> value) {
                    limits.depth = value;
                    depth_limited = true;
                }
                else if (token == "movetime" && iss >> value) limits.time_ms = value;
                else if (token == "wtime" && iss >> value) white_time = value;
                else if (token == "btime" && iss >> value) black_time = value;
                else if (token == "winc" && iss >> value) white_increment = value;
                else if (token == "binc" && iss >> value) black_increment = value;
                else if (token == "movestogo" && iss >> value) moves_to_go = std::max(1, value);
                else if (token == "infinite" || token == "ponder") infinite = true;
            }
            if (limits.time_ms == 0 && !infinite) {
                int remaining = pos.sideToMove == WHITE ? white_time : black_time;
                int increment = pos.sideToMove == WHITE ? white_increment : black_increment;
                if (remaining >= 0) {
                    limits.time_ms = std::max(1, std::min(remaining, remaining / moves_to_go + increment * 3 / 4));
                }
            }
            if (limits.time_ms > 0 && !depth_limited) limits.depth = Search::MAX_DEPTH;
            if (limits.depth < 1) limits.depth = Search::MAX_DEPTH;
            if (infinite) limits.depth = Search::MAX_DEPTH;
            Position search_position = pos;
            search_thread = std::thread([&, search_position, limits]() mutable {
                Search::Result result = Search::find_best_move(
                    search_position, limits, stop_search,
                    [&](const Search::Info& info) {
                        std::lock_guard<std::mutex> lock(output_mutex);
                        std::cout << "info depth " << info.depth << " score ";
                        if (std::abs(info.score) >= 29000) {
                            int mate = (30000 - std::abs(info.score) + 1) / 2;
                            std::cout << "mate " << (info.score < 0 ? -mate : mate);
                        } else {
                            std::cout << "cp " << info.score;
                        }
                        std::cout << " nodes " << info.nodes << " time " << info.time_ms << '\n' << std::flush;
                    });
                std::lock_guard<std::mutex> lock(output_mutex);
                std::cout << "bestmove " << (result.best_move.is_valid() ? move_to_string(result.best_move) : "0000")
                          << '\n' << std::flush;
            });
        }
        else if (command == "stop") {
            finish_search();
            stop_search.store(false, std::memory_order_relaxed);
        }
        else if (command == "quit") {
            finish_search();
            break;
        }
    }

    finish_search();
}

} // namespace UCI