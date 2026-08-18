#include "King_Moves.hpp"

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << sq))

U64 King_Moves::king_attacks[64];

U64 King_Moves::mask_king_attacks(int sq)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 bb = 1ULL << sq;
    U64 attacks = 0ULL;

    const U64 not_A_file = 18374403900871474942ULL;
    const U64 not_H_file = 9187201950435737471ULL;
    const U64 not_AB_file = 18229723555195321599ULL;
    const U64 not_GH_file = 4557430888798830399ULL;

    if (bb >> 8)
        attacks |= (bb >> 8);
    if ((bb & not_A_file) >> 9)
        attacks |= (bb & not_A_file) >> 9;
    if((bb & not_H_file) >> 7)
        attacks |= (bb& not_H_file) >> 7;
    if((attacks & not_A_file) >> 1)
        attacks |= (bb & not_A_file) >> 1;

    if(bb << 8)
        attacks |= (bb << 8);
    if((bb & not_A_file) << 7)
        attacks |= (bb & not_A_file) << 7;
    if((bb & not_H_file) << 9)
        attacks |= (bb & not_H_file) << 9;
    if((bb & not_H_file) << 1)
        attacks |= (bb & not_H_file) << 1;
    
    return attacks;
}

void King_Moves::init_king_attacks()
{
    for (int sq = 0; sq < 64; sq++)
    {
        king_attacks[sq] = mask_king_attacks(sq);
    }
}