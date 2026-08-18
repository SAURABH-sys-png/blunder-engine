#include "Bishop_Moves.hpp"

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << (sq)))

U64 Bishop_Moves::bishop_attacks[64];

U64 Bishop_Moves::mask_bishop_moves(int sq)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;

    int tr = sq / 8; // target rank
    int tf = sq % 8; // target file

    // North-East
    for (int r = tr + 1, f = tf + 1;
         r < 8 && f < 8;
         r++, f++)
    {
        set_Bit(attacks, r * 8 + f);
    }

    // North-West
    for (int r = tr + 1, f = tf - 1;
         r < 8 && f >= 0;
         r++, f--)
    {
        set_Bit(attacks, r * 8 + f);
    }

    // South-East
    for (int r = tr - 1, f = tf + 1;
         r >= 0 && f < 8;
         r--, f++)
    {
        set_Bit(attacks, r * 8 + f);
    }

    // South-West
    for (int r = tr - 1, f = tf - 1;
         r >= 0 && f >= 0;
         r--, f--)
    {
        set_Bit(attacks, r * 8 + f);
    }

    return attacks;
}

void Bishop_Moves::init_bishop_moves()
{
    for (int sq = 0; sq < 64; sq++)
    {
        bishop_attacks[sq] = mask_bishop_moves(sq);
    }
}