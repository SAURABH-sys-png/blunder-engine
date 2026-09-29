#include "UCI.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <random>

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
    std::mt19937 rng(std::random_device{}());

    // Disable C++ I/O synchronization with C stdio for faster, predictable flushing
    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "uci") {
            std::cout << "id name MyBitboardEngine\n";
            std::cout << "id author Developer\n";
            std::cout << "uciok\n" << std::flush;
        }
        else if (command == "isready") {
            std::cout << "readyok\n" << std::flush;
        }
        else if (command == "ucinewgame") {
            pos.set_startpos();
        }
        else if (command == "position") {
            std::string type;
            iss >> type;

            if (type == "startpos") {
                pos.set_startpos();
                std::string moves_token;
                if (iss >> moves_token && moves_token == "moves") {
                    std::string move_token;
                    while (iss >> move_token) {
                        Move m = parse_move(pos, move_token);
                        pos.make_move(m);
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
                        pos.make_move(m);
                    }
                }
            }
        }
        else if (command == "go") {
            std::vector<Move> legal_moves;
            pos.generate_legal_moves(legal_moves);

            if (legal_moves.empty()) {
                // Null move when in checkmate or stalemate
                std::cout << "bestmove 0000\n" << std::flush;
            } else {
                std::uniform_int_distribution<size_t> dist(0, legal_moves.size() - 1);
                Move chosen = legal_moves[dist(rng)];
                std::cout << "bestmove " << move_to_string(chosen) << "\n" << std::flush;
            }
        }
        else if (command == "quit") {
            break;
        }
    }
}

} // namespace UCI