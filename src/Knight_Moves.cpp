#include "Knight_Moves.hpp"

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << sq))

U64 Knight_Moves::knight_attacks[64];

U64 Knight_Moves::mask_knight_attacks(int sq)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 bb = 1ULL << sq;
    U64 attacks = 0ULL;

    const U64 not_A_file = 18374403900871474942ULL;
    const U64 not_H_file = 9187201950435737471ULL;
    const U64 not_AB_file = 18229723555195321599ULL;
    const U64 not_GH_file = 4557430888798830399ULL;

    if (sq >= 17)
        attacks |= (bb >> 17) & not_H_file;

    if (sq >= 15)
        attacks |= (bb >> 15) & not_A_file;

    if (sq >= 10)
        attacks |= (bb >> 10) & not_GH_file;

    if (sq >= 6)
        attacks |= (bb >> 6) & not_AB_file;

    if (sq <= 46)
        attacks |= (bb << 17) & not_A_file;

    if (sq <= 48)
        attacks |= (bb << 15) & not_H_file;

    if (sq <= 53)
        attacks |= (bb << 10) & not_AB_file;

    if (sq <= 57)
        attacks |= (bb << 6) & not_GH_file;

    return attacks;
}

void Knight_Moves::init_knight_attacks()
{
    for (int sq = 0; sq < 64; sq++)
    {
        knight_attacks[sq] = mask_knight_attacks(sq);
    }
}
