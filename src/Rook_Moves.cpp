#include "Rook_Moves.hpp"
#include "Bishop_Moves.hpp"
#include "Magic_Numbers.hpp"

#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << (sq)))

U64 Rook_Moves::rook_attacks[64];
U64 Rook_Moves::rook_masks[64];

U64 Rook_Moves::rook_actual_attacks[64][4096];
const int Rook_Moves::relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};
U64 Rook_Moves::magic_number[64] = {
  2384658201720259592ULL, 563036926820738ULL, 235385648901526536ULL, 126674734669676704ULL,
  40552462884929540ULL, 2323936573164765184ULL, 18016323460141057ULL, 598135433859217ULL,
  5188155841970258498ULL, 7638194861690126848ULL, 11529250235826839552ULL, 4611972587374247936ULL,
  252207626460468288ULL, 20275546321617675ULL, 581246102894805312ULL, 20970245201946ULL,
  9525782000575732768ULL, 145241715048458242ULL, 603554918944489488ULL, 9520609655487730688ULL,
  9371146477206798336ULL, 55240567990583392ULL, 2666130990811856896ULL, 288230515781142784ULL,
  35390681584648ULL, 288797730603010246ULL, 1450176676628537344ULL, 228698435354688ULL,
  6917669765397991460ULL, 3131483208294416ULL, 2308377523106358176ULL, 324822123124131944ULL,
  37159094974562952ULL, 4611967665362386944ULL, 8892463106ULL, 2491098071040ULL,
  4538785207438336ULL, 2882333450344857600ULL, 4683763962298241062ULL, 6070852360192690304ULL,
  90652554014233088ULL, 4612254466627346496ULL, 4919197705496431904ULL, 2323910192367927552ULL,
  6990748118291579008ULL, 2473155047199232ULL, 6127229228158158858ULL, 10664664999244862978ULL,
  313544151700ULL, 19792352280896ULL, 576460791109189632ULL, 2305878194693090304ULL,
  9223653517373210625ULL, 649362840007880704ULL, 360856173414400ULL, 6755435482841089ULL,
  9530267741731160085ULL, 37163682001588228ULL, 2269941765506048ULL, 1730085945694360832ULL,
  4538786419606288ULL, 2251806273045012ULL, 2315219657260269717ULL, 2041321191669781ULL
};

void Rook_Moves::init_magic_numbers()
{
    for(int sq = 0;sq<64;sq++){
        magic_number[sq] = Magic_Numbers::generate_Magic_Num(sq,relevant_bits[sq],0);
    }
}

void Rook_Moves::init_magic_attack_table()
{
    for(int sq = 0;sq<64;sq++){
        Bishop_Moves::bishop_masks[sq] = Bishop_Moves::mask_bishop_moves(sq);
        rook_masks[sq] = mask_rook_attacks(sq);
        U64 attack_mask = rook_masks[sq];
        int relevant_bits_count = Magic_Numbers::count_bits(attack_mask);
        int occupancy_indices = (1 << relevant_bits_count);

        for(int idx = 0;idx<occupancy_indices;idx++){
            U64 occupancy = Magic_Numbers::set_occupancy(idx,relevant_bits_count,attack_mask);
            int mafic_index = (occupancy*magic_number[sq] >> (64-relevant_bits[sq]));
            rook_actual_attacks[sq][mafic_index] = rook_attacks_on_the_fly(sq,occupancy);
        }
    }
}

U64 Rook_Moves::get_rook_attacks(int sq, U64 occupancy)
{
    occupancy &= rook_masks[sq];
    occupancy *= magic_number[sq];
    occupancy >>=64-relevant_bits[sq];
    return rook_actual_attacks[sq][occupancy];
}

U64 Rook_Moves::mask_rook_attacks(int sq)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;

    int rank = sq / 8;
    int file = sq % 8;

    for (int r = rank + 1; r <= 6; r++)
        attacks |= (1ULL << (r * 8 + file));
    for (int r = rank - 1; r >= 1; r--)
        attacks |= (1ULL << (r * 8 + file));
    for (int f = file + 1; f <= 6; f++)
        attacks |= (1ULL << (rank * 8 + f));
    for (int f = file - 1; f >= 1; f--)
        attacks |= (1ULL << (rank * 8 + f));
    return attacks;
}

// Precompute things up
void Rook_Moves::init_rook_attacks()
{
    for (int sq = 0; sq < 64; ++sq)
    {
        rook_attacks[sq] = mask_rook_attacks(sq);
    }
}

// moves on the gly

U64 Rook_Moves::rook_attacks_on_the_fly(int sq, U64 blocks)
{
    if (sq < 0 || sq >= 64)
        return 0ULL;

    U64 attacks = 0ULL;

    int rank = sq / 8;
    int file = sq % 8;

    for (int r = rank + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + file));
        if ((1ULL << (r * 8 + file)) & blocks)
            break;
    }
    for (int r = rank - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + file));
        if ((1ULL << (r * 8 + file)) & blocks)
            break;
    }
    for (int f = file + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (rank * 8 + f));
        if ((1ULL << (rank * 8 + f)) & blocks)
            break;
    }
    for (int f = file - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (rank * 8 + f));
        if ((1ULL << (rank * 8 + f)) & blocks)
            break;
    }
    return attacks;
}