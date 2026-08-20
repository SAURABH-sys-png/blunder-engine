#include "Rook_Moves.hpp"

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << (sq)))

U64 Rook_Moves::rook_attacks[64];

U64 Rook_Moves::mask_rook_attacks(int sq)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;

    int rank = sq / 8;
    int file = sq % 8;

    for (int r = rank + 1; r <= 6; r++)
        attacks |= (1ULL << (r * 8 + file));
    for (int r = rank - 1; r >= 1; r--)
        attacks |= (1ULL << (r * 8 + file));
    for (int f = file + 1; f <= 6; f++)
        attacks |= (1ULL << (rank * 8 + f));
    for (int f = file - 1; f >= 1; f--)
        attacks |= (1ULL << (rank * 8 + f));
    return attacks;
}

// Precompute things up
void Rook_Moves::init_rook_attacks()
{
    for (int sq = 0; sq < 64; ++sq)
    {
        rook_attacks[sq] = mask_rook_attacks(sq);
    }
}

// moves on the gly

U64 Rook_Moves::rook_attacks_on_the_fly(int sq, U64 blocks)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;

    int rank = sq / 8;
    int file = sq % 8;

    for (int r = rank + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + file));
        if ((1ULL << (r * 8 + file)) & blocks)
            break;
    }
    for (int r = rank - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + file));
        if ((1ULL << (r * 8 + file)) & blocks)
            break;
    }
    for (int f = file + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (rank * 8 + f));
        if ((1ULL << (rank * 8 + f)) & blocks)
            break;
    }
    for (int f = file - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (rank * 8 + f));
        if ((1ULL << (rank * 8 + f)) & blocks)
            break;
    }
    return attacks;
}