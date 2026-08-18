#pragma once

#include <iostream>

#define U64 unsigned long long	
class Bishop_Moves{
	public:
		static U64 bishop_attacks[64];
	public:
		static U64 mask_bishop_moves(int sq);
		static void init_bishop_moves();
};
