#include "Pawn_Moves.hpp"
enum{
	white,
	black
};
const U64 not_A_file = 18374403900871474942ULL;
const U64 not_H_file = 9187201950435737471ULL;

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << sq))

U64 Pawn_Moves::pawn_attacks[2][64];

U64 Pawn_Moves::mask_pawn_attacks(int sq, int side)
{
	U64 attacks = 0ULL;
	U64 bitboard = 0ULL;
	set_Bit(bitboard, sq);

	if (!side)
	{
		attacks = (bitboard & not_H_file) >> 7;
		attacks |= (bitboard & not_A_file) >> 9;
	}
	else
	{
		attacks = (bitboard & not_A_file) << 7;
		attacks |= (bitboard & not_H_file) << 9;
	}
	return attacks;
}


void Pawn_Moves::init_pawn_attacks(){
	for (int sq = 0; sq< 64; sq++)
	{
		pawn_attacks[white][sq] = mask_pawn_attacks(sq,white);
		pawn_attacks[black][sq] = mask_pawn_attacks(sq,black);
	}
	
}
