#pragma once

#define U64 unsigned long long

class Rook_Moves
{
public:
	static U64 rook_attacks[64];

public:
	static U64 mask_rook_attacks(int sq);
	static void init_rook_attacks();
	static U64 rook_attacks_on_the_fly(int sq,U64 blocks);
};