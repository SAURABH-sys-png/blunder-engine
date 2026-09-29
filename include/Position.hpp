#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <cstdint>

using U64 = unsigned long long;

enum Color : int {
    WHITE = 0,
    BLACK = 1
};

enum Pieces : int {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
};

enum Occu : int {
    WHITE_OC = 0,
    BLACK_OC = 1,
    BOTH_OC = 2
};

enum CastlingRights : int {
    WK = 1,
    WQ = 2,
    BK = 4,
    BQ = 8
};

// Constant for invalid / no square (e.g., en passant disabled)
inline constexpr int NO_SQ = 64;

// Lightweight 16-bit or 32-bit Move struct
struct Move {
    uint16_t data = 0;

    // Bit layout:
    // 0-5:   from square (0-63)
    // 6-11:  to square (0-63)
    // 12-13: promotion piece type (0: Knight, 1: Bishop, 2: Rook, 3: Queen)
    // 14:    is promotion flag
    // 15:    is capture flag (optional metadata)

    Move() = default;
    Move(int from, int to, int promo_piece = -1, bool is_capture = false) {
        data = (from & 0x3F) | ((to & 0x3F) << 6);
        if (promo_piece != -1) {
            data |= (1 << 14); // promotion flag
            data |= ((promo_piece & 0x3) << 12);
        }
        if (is_capture) {
            data |= (1 << 15);
        }
    }

    [[nodiscard]] int from_square() const { return data & 0x3F; }
    [[nodiscard]] int to_square() const { return (data >> 6) & 0x3F; }
    [[nodiscard]] bool is_promotion() const { return (data & (1 << 14)) != 0; }
    [[nodiscard]] bool is_capture() const { return (data & (1 << 15)) != 0; }

    [[nodiscard]] char promotion_char() const {
        if (!is_promotion()) return ' ';
        int promo_type = (data >> 12) & 0x3;
        switch (promo_type) {
            case 0: return 'n';
            case 1: return 'b';
            case 2: return 'r';
            case 3: default: return 'q';
        }
    }

    bool operator==(const Move& other) const { return data == other.data; }
    bool operator!=(const Move& other) const { return data != other.data; }

    static Move none() { return Move(); }
    [[nodiscard]] bool is_valid() const { return data != 0; }
};

class Position {
public:
    // Bitboard state matrices
    U64 state[2][6];    // [color][piece]
    U64 occupancies[3]; // [WHITE_OC, BLACK_OC, BOTH_OC]

    // Game state variables
    int sideToMove;      // 0 = White, 1 = Black
    int enPassantSquare; // 0-63, or NO_SQ (64)
    int castlingRights;  // 4-bit mask (WK=1, WQ=2, BK=4, BQ=8)
    int halfMoveClock;   // 50-move rule counter
    int fullMoveNumber;  // Starts at 1, increments after Black's move

public:
    Position();

    // Board setup & state synchronization
    void resetEverything();
    void set_startpos();
    void set_from_fen(std::string fen);
    void updateOccupancies();

    // Attack detection
    [[nodiscard]] bool is_square_attacked(int sq, int attacking_color) const;
    [[nodiscard]] bool is_in_check(int color) const;

    // Move making & unmaking
    bool make_move(const Move& m); // Returns false if move left king in check
    void make_move_uci(std::string moveStr); // Helper to parse string and make move

    // Move generation
    void generate_legal_moves(std::vector<Move>& move_list);
    void generate_pseudo_legal_moves(std::vector<Move>& move_list);

    // Debugging
    void print_board() const;
};
