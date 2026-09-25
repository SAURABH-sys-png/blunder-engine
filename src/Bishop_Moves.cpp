#include "Bishop_Moves.hpp"
#include "Magic_Numbers.hpp"
#include "Rook_Moves.hpp"

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << (sq)))

U64 Bishop_Moves::bishop_attacks[64];
U64 Bishop_Moves::bishop_masks[64];

U64 Bishop_Moves::bishop_actual_attacks[64][512];
const int Bishop_Moves::relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};
U64 Bishop_Moves::magic_number[64] = {
  11575658452893700ULL, 7077010911297920ULL, 72218176692682840ULL, 18094662864601088ULL,
  438925656096ULL, 3603442876281831424ULL, 867365214016765984ULL, 594477351715213314ULL,
  13186539709279520842ULL, 4364273160ULL, 2309858580834033676ULL, 13549350508560896ULL,
  5765742220208505856ULL, 4620746269523181696ULL, 1154973201904697600ULL, 10423689402123264ULL,
  387344758976053264ULL, 163822909654433792ULL, 146649014769549312ULL, 2308094843588445312ULL,
  2252907940430344ULL, 288265706825490432ULL, 9575994683818256ULL, 9817851822206877954ULL,
  153131183458713856ULL, 35201652655136ULL, 4760322501412732928ULL, 36319660802572288ULL,
  633336011686049ULL, 3531385057813040752ULL, 1409573923588324ULL, 595671428053860715ULL,
  1762518231941189ULL, 1369376449161138371ULL, 882992501714059288ULL, 37154838659989540ULL,
  1152965485074074636ULL, 2306125244433171458ULL, 4786534905677824ULL, 8796630000672ULL,
  1002771784405248ULL, 1203966308384928ULL, 6935968942586855424ULL, 19316363092828164ULL,
  2308094843588445312ULL, 1154127191048454272ULL, 577622532456185856ULL, 360878408493711360ULL,
  2308464313805053960ULL, 5769254059175610368ULL, 4791830209699518466ULL, 36028797290184704ULL,
  1766677829870757504ULL, 4611829092380090368ULL, 36169946846396544ULL, 140883559264272ULL,
  2305843013514053633ULL, 4413750018112ULL, 36029347353993276ULL, 148760627460538368ULL,
  1171507656477704704ULL, 1226360203495105024ULL, 2200210785283ULL, 361486442161766656ULL
};

void Bishop_Moves::init_magic_numbers()
{
    for(int sq = 0;sq<64;sq++){
        magic_number[sq] = Magic_Numbers::generate_Magic_Num(sq,relevant_bits[sq],1);
    }
}

void Bishop_Moves::init_magic_attack_table()
{
    for(int sq = 0;sq<64;sq++){
        bishop_masks[sq] = mask_bishop_moves(sq);
        Rook_Moves::rook_masks[sq] = Rook_Moves::mask_rook_attacks(sq);
        U64 attack_mask = bishop_masks[sq];
        int relevant_bits_count = Magic_Numbers::count_bits(attack_mask);
        int occupancy_indices = (1 << relevant_bits_count);

        for(int idx = 0;idx<occupancy_indices;idx++){
            U64 occupancy = Magic_Numbers::set_occupancy(idx,relevant_bits_count,attack_mask);
            int mafic_index = (occupancy*magic_number[sq] >> (64-relevant_bits[sq]));
            bishop_actual_attacks[sq][mafic_index] = bishop_attacks_on_the_fly(sq,occupancy);
        }
    }
}

U64 Bishop_Moves::get_bishop_attacks(int sq, U64 occupancy)
{
    occupancy &= bishop_masks[sq];
    occupancy *= magic_number[sq];
    occupancy >>=64-relevant_bits[sq];
    return bishop_actual_attacks[sq][occupancy];
}

U64 Bishop_Moves::mask_bishop_moves(int sq)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;
    int r, f;
    int tr = sq / 8; // target rank
    int tf = sq % 8; // target file

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
        attacks |= (1ULL << (r * 8 + f));
    return attacks;
}

void Bishop_Moves::init_bishop_moves()
{
    for (int sq = 0; sq < 64; sq++)
    {
        bishop_attacks[sq] = mask_bishop_moves(sq);
    }
}

U64 Bishop_Moves::bishop_attacks_on_the_fly(int sq, U64 blocks)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;
    int r, f;
    int tr = sq / 8; // target rank
    int tf = sq % 8; // target file

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r*8+f))& blocks)break;
    }
    return attacks;
}