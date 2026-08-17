#pragma once

#define U64 unsigned long long

class Pawn_Moves
{
public:
	static U64 pawn_attacks[2][64];

public:
	static U64 mask_pawn_attacks(int sq, int side);
	static void init_pawn_attacks();
};
