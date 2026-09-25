#include "Magic_Numbers.hpp"

#include "Bishop_Moves.hpp"
#include "Rook_Moves.hpp"

#include <cstdio>
#include <cstring>

namespace Magic_Numbers
{
namespace
{
unsigned int state = 1806358902;

unsigned int get_random_num()
{
  unsigned int num = state;

  num ^= num << 13;
  num ^= num >> 17;
  num ^= num << 5;

  state = num;
  return num;
}

Bitboard getRandom_U64_nums()
{
  Bitboard n1,n2,n3,n4;

  n1 = (Bitboard)(get_random_num() & 0xFFFF);
  n2 = (Bitboard)(get_random_num() & 0xFFFF);
  n3 = (Bitboard)(get_random_num() & 0xFFFF);
  n4 = (Bitboard)(get_random_num() & 0xFFFF);

  return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

Bitboard generate_Magic_Num_Candidate()
{
  return (getRandom_U64_nums()&getRandom_U64_nums()&getRandom_U64_nums());
}

int get_ls1b_index(Bitboard bitboard)
{
  if (bitboard) {
    return count_bits((bitboard & -bitboard)-1);
  }
  else {
    return -1;
  }
}
}

int count_bits(Bitboard bitboard)
{
  int cnt = 0;

  while (bitboard) {
    cnt++;
    bitboard &= bitboard-1;
  }
  return cnt;
}

Bitboard set_occupancy(int idx, int bits_in_mask, Bitboard attack_mask)
{
  Bitboard occupancy = 0ULL;
  for (int cnt = 0; cnt < bits_in_mask; cnt++) {
    int sq = get_ls1b_index(attack_mask);
    attack_mask &= ~(1ULL << sq);
    if (idx & (1<<cnt)) {
      occupancy |= (1ULL << sq);
    }
  }
  return occupancy;
}

Bitboard generate_Magic_Num(int square, int relevant_bits, int bishop)
{
  Bitboard occupancy[4096];
  Bitboard attacks[4096];
  Bitboard used_attacks[4096];

  Bitboard attack_mask = bishop ? Bishop_Moves::mask_bishop_moves(square) : Rook_Moves::mask_rook_attacks(square);

  Bitboard occupancy_indices = 1ULL << relevant_bits;

  for(int index = 0;index<occupancy_indices;index++){
    occupancy[index] = set_occupancy(index,relevant_bits,attack_mask);
    attacks[index] = bishop ? Bishop_Moves::bishop_attacks_on_the_fly(square,occupancy[index]) : Rook_Moves::rook_attacks_on_the_fly(square,occupancy[index]);
  }

  for(int random_count = 0;random_count < 100000000;random_count++){
    Bitboard magic_number = generate_Magic_Num_Candidate();
    if(count_bits((attack_mask * magic_number)&0xFF00000000000000) < 6)continue;
    memset(used_attacks,0ULL,sizeof(used_attacks));
    int fail = 0;

    for(int index = 0;!fail && index < occupancy_indices;index++){
      int magic_index = (int)((occupancy[index] * magic_number) >> (64 - relevant_bits));
      if(used_attacks[magic_index] == 0ULL){
        used_attacks[magic_index] = attacks[index];
      }
      else if(used_attacks[magic_index] != attacks[index])
      {
        fail = 1;
      }
    }
    if(!fail){
      return magic_number;
    }
  }
  printf("   Magic NUmber fails! ");
  return 0ULL;
}
}