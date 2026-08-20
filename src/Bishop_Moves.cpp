#include "Bishop_Moves.hpp"

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << (sq)))

U64 Bishop_Moves::bishop_attacks[64];

U64 Bishop_Moves::mask_bishop_moves(int sq)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;
    int r, f;
    int tr = sq / 8; // target rank
    int tf = sq % 8; // target file

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
        attacks |= (1ULL << (r * 8 + f));
    return attacks;
}

void Bishop_Moves::init_bishop_moves()
{
    for (int sq = 0; sq < 64; sq++)
    {
        bishop_attacks[sq] = mask_bishop_moves(sq);
    }
}

U64 Bishop_Moves::bishop_attacks_on_the_fly(int sq, U64 blocks)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;
    int r, f;
    int tr = sq / 8; // target rank
    int tf = sq % 8; // target file

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }
    return attacks;
}