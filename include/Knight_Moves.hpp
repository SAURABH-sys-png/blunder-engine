#pragma once

#define U64 unsigned long long

class Knight_Moves
{
public:
	static U64 knight_attacks[64];

public:
	static U64 mask_knight_attacks(int sq);
	static void init_knight_attacks();
};
