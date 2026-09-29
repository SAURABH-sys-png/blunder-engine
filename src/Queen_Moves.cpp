#include "Queen_Moves.hpp"

class Queen_Moves :public Rook_Moves,public Bishop_Moves
{
public:
    static U64 get_Queen_Moves(int sq,U64 occupancy){
        return (get_rook_attacks(sq,occupancy) | get_bishop_attacks(sq,occupancy));
    }
};