#pragma once

namespace Magic_Numbers
{
using Bitboard = unsigned long long;

int count_bits(Bitboard bitboard);
Bitboard set_occupancy(int idx, int bits_in_mask, Bitboard attack_mask);
Bitboard generate_Magic_Num(int square, int relevant_bits, int bishop);
}