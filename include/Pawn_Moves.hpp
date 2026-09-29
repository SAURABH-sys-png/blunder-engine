#pragma once

#include "Position.hpp"

// Board Rank Bitmasks for Double Push verification
// Rank 2: squares 48..55 | Rank 7: squares 8..15
constexpr U64 RANK_2 = 0x000000000000FF00ULL;
constexpr U64 RANK_7 = 0x00FF000000000000ULL;

class Pawn_Moves
{
public:
    // Precalculated lookup table for raw attack masks [color][square]
    static U64 pawn_attacks[2][64];

    // Initialization
    static void init_pawn_attacks();
    static U64 mask_pawn_attacks(int sq, int side);

    // 1. ATTACKS / CAPTURES
    // Gets raw pseudo-attacks for a square (used for check detection & bitboard generation)
    static U64 get_pawn_attacks(int sq, int side);
    
    // Gets executable captures (Enemy pieces + En Passant square)
    static U64 get_pawn_captures(int sq, int side, U64 enemy_occupancy, int enPassantSquare);

    // 2. QUIET PUSHES
    // Gets single and double pushes into empty squares
    static U64 get_pawn_pushes(int sq, int side, U64 both_occupancy);

    // 3. COMBINED MOVESET
    // Merges pushes and captures for a specific pawn
    static U64 get_pawn_moves(int sq, int side, U64 both_occupancy, U64 enemy_occupancy, int enPassantSquare);
};
