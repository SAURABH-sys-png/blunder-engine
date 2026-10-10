#pragma once

#include <string>
#include "Position.hpp"

namespace UCI {
    // Converts internal move to standard algebraic format (e.g. e2e4, e7e8q)
    std::string move_to_string(const Move& m);

    // Parses an algebraic move string received from the GUI against current legal moves
    Move parse_move(Position& pos, const std::string& move_str);

    // Starts the blocking I/O loop communicating with the GUI over standard input/output
    void loop();
}
