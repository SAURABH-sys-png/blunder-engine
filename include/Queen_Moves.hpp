#pragma once
#include <bits/stdc++.h>
#include "Rook_Moves.hpp"
#include "Bishop_Moves.hpp"
class Queen_Moves :public Rook_Moves ,public Bishop_Moves
{
public:
    static U64 get_Queen_Moves(int sq,U64 occupancy);
};