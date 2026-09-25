#pragma once

#include <iostream>

#define U64 unsigned long long	
class Bishop_Moves{
	private:
		static const int relevant_bits[64];
		static U64 magic_number[64];
	public:
		static U64 bishop_attacks[64];
		static U64 bishop_masks[64];
		// bishop attacks table
		static U64 bishop_actual_attacks[64][512];
	public:
		static U64 mask_bishop_moves(int sq);
		static void init_bishop_moves();
		static U64 bishop_attacks_on_the_fly(int sq, U64 blocks);
		static void init_magic_numbers();
		static void init_magic_attack_table();
		static U64 get_bishop_attacks(int sq, U64 occupancy);
};
