#include <bits/stdc++.h>
#include "Pawn_Moves.hpp"
#include "Knight_Moves.hpp"
#include "King_Moves.hpp"
#include "Bishop_Moves.hpp"
#include "Rook_Moves.hpp"
// board declarations
#define U64 unsigned long long

// not a file
const U64 not_A_file = 18374403900871474942ULL;
const U64 not_H_file = 9187201950435737471ULL;
const U64 not_HG_file = 4557430888798830399ULL;
const U64 not_AB_file = 18229723555140126207ULL;
// Board square representation
enum
{
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

const char* sq_to_coordinates[64] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};
enum
{
	white,
	black
};

// bitmanipulations
#define get_Bit(BitBoard, sq) (BitBoard & (1ULL << sq))
#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << sq))
#define clear_Bit(BitBoard, sq) (BitBoard &= ~(1ULL << sq))

// Counting significat bits
static inline int count_bits(U64 bitBoard){
  // bit bishop_attacks_on_the_fly
  int cnt = 0;

  while (bitBoard) {
    //increment count_bicnt++;
    cnt++;
    bitBoard &= bitBoard-1;
  }
  return cnt;
}

// get least significat bit
static inline int get_ls1b_index(U64 bitBoard){
  if (bitBoard) {
    //get get_ls1b_index
    return count_bits((bitBoard & -bitBoard)-1);
  }
  else {
    return -1;
  }
}


U64 set_occupancy(int idx,int bits_in_mask,U64 attack_mask){
  U64 occupancy = 0ULL;
  for (int cnt = 0; cnt < bits_in_mask; cnt++) {
    int sq = get_ls1b_index(attack_mask);
    clear_Bit(attack_mask,sq);
    if (idx & (1<<cnt)) {
      //populating occupancy
      occupancy |= (1ULL << sq);
    }
  }
  return occupancy;
}


// Printing the Board
void print_Board(U64 BitBoard)
{
	std::cout << "\n";
	std::cout << "    +---+---+---+---+---+---+---+---+\n";

	for (int rank = 0; rank < 8; rank++)
	{
		std::cout << "  " << (8 - rank) << " |";
		for (int file = 0; file < 8; file++)
		{
			int sq = rank * 8 + file;
			std::cout << " " << (get_Bit(BitBoard, sq) ? 1 : 0) << " |";
		}
		std::cout << "\n";
		std::cout << "    +---+---+---+---+---+---+---+---+\n";
	}
	std::cout << "      a   b   c   d   e   f   g   h\n";

	std::cout << "\nBitBoard: " << std::bitset<64>(BitBoard) << "\n";
}

int main()
{
  U64 attack_mask =  Rook_Moves::mask_rook_attacks(a1);

  U64 occupancy = set_occupancy(4095,count_bits(attack_mask),attack_mask);
	print_Board(occupancy);
	
	return 0;
}
