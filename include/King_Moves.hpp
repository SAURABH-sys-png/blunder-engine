#pragma once

#include "iostream"
#define U64 unsigned long long
class King_Moves
{
public:
    static U64 king_attacks[64];

public:
    static U64 mask_king_attacks(int sq);
    static void init_king_attacks();
};