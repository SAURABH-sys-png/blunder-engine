#pragma once

#include <string_view>

using U64 = unsigned long long;

enum Color
{
    WHITE,
    BLACK
};

enum Pieces
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Occu
{
    WHITE_OC,
    BLACK_OC,
    BOTH_OC
};

enum CastlingRights
{
    WK = 1,
    WQ = 2,
    BK = 4,
    BQ = 8
};

class Position
{
public:
    // Bitboard state matrices
    U64 state[2][6]; // [color][piece]
    U64 occupancies[3];

    // Game state variables
    int sideToMove;      // 0 = White, 1 = Black
    int enPassantSquare; // 0-63, or 64/no_sq if not available
    int castlingRights;  // 4-bit mask (15 = 1111 = all rights available)
    int halfMoveClock;   // 50-move rule counter
    int fullMoveNumber;  // Starts at 1, increments after Black's move

public:
    Position();

    void resetEverything();
    void updateOccupencies();
    void MakeMove(std::string_view moveStr);
};

