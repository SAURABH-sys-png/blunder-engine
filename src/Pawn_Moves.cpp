#include "Pawn_Moves.hpp"

// Define static lookup table
U64 Pawn_Moves::pawn_attacks[2][64];

// Mask helper functions
const U64 not_A_file = 18374403900871474942ULL;
const U64 not_H_file = 9187201950435737471ULL;

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << sq)) // already declared in the main.cpp make it use that directly 

U64 Pawn_Moves::mask_pawn_attacks(int sq, int side)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_Bit(bitboard, sq);

    if (side == WHITE)
    {
        // Up-right and up-left (rank + 1)
        attacks = (bitboard & not_H_file) << 9;
        attacks |= (bitboard & not_A_file) << 7;
    }
    else
    {
        // Down-left and down-right (rank - 1)
        attacks = (bitboard & not_H_file) >> 7;
        attacks |= (bitboard & not_A_file) >> 9;
    }
    return attacks;
}

void Pawn_Moves::init_pawn_attacks()
{
    for (int sq = 0; sq < 64; sq++)
    {
        pawn_attacks[WHITE][sq] = mask_pawn_attacks(sq, WHITE);
        pawn_attacks[BLACK][sq] = mask_pawn_attacks(sq, BLACK);
    }
}

// ----------------------------------------------------------------------------
// 1. Attack / Capture Generation
// ----------------------------------------------------------------------------

U64 Pawn_Moves::get_pawn_attacks(int sq, int side)
{
    return pawn_attacks[side][sq];
}

U64 Pawn_Moves::get_pawn_captures(int sq, int side, U64 enemy_occupancy, int enPassantSquare)
{
    U64 valid_captures = 0ULL;
    U64 raw_attacks = pawn_attacks[side][sq];

    // Standard captures (must land on an enemy-occupied square)
    valid_captures |= (raw_attacks & enemy_occupancy);

    // En Passant capture (if EP target square is active and under attack by this pawn)
    if (enPassantSquare >= 0 && enPassantSquare < 64)
    {
        U64 ep_bitboard = (1ULL << enPassantSquare);
        valid_captures |= (raw_attacks & ep_bitboard);
    }

    return valid_captures;
}

// ----------------------------------------------------------------------------
// 2. Quiet Push Generation
// ----------------------------------------------------------------------------

U64 Pawn_Moves::get_pawn_pushes(int sq, int side, U64 both_occupancy)
{
    U64 pushes = 0ULL;

    if (side == WHITE)
    {
        int single_sq = sq + 8;
        // Check single push (target square must be completely empty)
        if (single_sq < 64 && !(both_occupancy & (1ULL << single_sq)))
        {
            pushes |= (1ULL << single_sq);

            // Check double push (must start on Rank 2 and intermediate + target squares empty)
            int double_sq = sq + 16;
            if ((1ULL << sq) & RANK_2)
            {
                if (!(both_occupancy & (1ULL << double_sq)))
                {
                    pushes |= (1ULL << double_sq);
                }
            }
        }
    }
    else // BLACK
    {
        int single_sq = sq - 8;
        // Check single push
        if (single_sq >= 0 && !(both_occupancy & (1ULL << single_sq)))
        {
            pushes |= (1ULL << single_sq);

            // Check double push (must start on Rank 7)
            int double_sq = sq - 16;
            if ((1ULL << sq) & RANK_7)
            {
                if (!(both_occupancy & (1ULL << double_sq)))
                {
                    pushes |= (1ULL << double_sq);
                }
            }
        }
    }

    return pushes;
}

// ----------------------------------------------------------------------------
// 3. Combined Moveset
// ----------------------------------------------------------------------------

U64 Pawn_Moves::get_pawn_moves(int sq, int side, U64 both_occupancy, U64 enemy_occupancy, int enPassantSquare)
{
    return get_pawn_pushes(sq, side, both_occupancy) | 
           get_pawn_captures(sq, side, enemy_occupancy, enPassantSquare);
}
