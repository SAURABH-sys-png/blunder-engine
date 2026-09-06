#include <bits/stdc++.h>
// Include the move-generation interfaces used by this test program.
#include "Pawn_Moves.hpp"
#include "Knight_Moves.hpp"
#include "King_Moves.hpp"
#include "Bishop_Moves.hpp"
#include "Rook_Moves.hpp"
// Use a short name for the 64-bit bitboard type.
#define U64 unsigned long long


// Number of relevant blocker squares for bishop mask generation.
const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5 ,5 ,6
};

// Number of relevant blocker squares for rook mask generation.
const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};
// File masks used to prevent attacks from wrapping around board edges.
const U64 not_A_file = 18374403900871474942ULL;
const U64 not_H_file = 9187201950435737471ULL;
const U64 not_HG_file = 4557430888798830399ULL;
const U64 not_AB_file = 18229723555140126207ULL;
// Map each board square to an index from a8 (0) through h1 (63).
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
  // Numeric colors used when selecting a side to move.
	white,
	black
};

// Macros for testing, setting, and clearing individual bits in a bitboard.
#define get_Bit(BitBoard, sq) (BitBoard & (1ULL << sq))
#define set_Bit(BitBoard, sq) (BitBoard |= (1ULL << sq))
#define clear_Bit(BitBoard, sq) (BitBoard &= ~(1ULL << sq))

// Count the number of set bits in a bitboard.
static inline int count_bits(U64 bitBoard){
  int cnt = 0;

  // Remove the lowest set bit on each iteration until the board is empty.
  while (bitBoard) {
    cnt++;
    bitBoard &= bitBoard-1;
  }
  return cnt;
}

// Return the index of the least significant set bit, or -1 for an empty board.
static inline int get_ls1b_index(U64 bitBoard){
  if (bitBoard) {
    return count_bits((bitBoard & -bitBoard)-1);
  }
  else {
    return -1;
  }
}


// Build one occupancy bitboard from the selected subset of an attack mask.
U64 set_occupancy(int idx,int bits_in_mask,U64 attack_mask){
  U64 occupancy = 0ULL;
  // Visit each relevant blocker square and copy it when its subset bit is set.
  for (int cnt = 0; cnt < bits_in_mask; cnt++) {
    int sq = get_ls1b_index(attack_mask);
    clear_Bit(attack_mask,sq);
    if (idx & (1<<cnt)) {
      occupancy |= (1ULL << sq);
    }
  }
  return occupancy;
}


// Print a bitboard as an 8x8 board and then display its 64-bit representation.
void print_Board(U64 BitBoard)
{
  // Print the board border above the first rank.
	std::cout << "\n";
	std::cout << "    +---+---+---+---+---+---+---+---+\n";

	for (int rank = 0; rank < 8; rank++)
	{
		std::cout << "  " << (8 - rank) << " |";
    // Print the eight files belonging to the current rank.
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

// get random number
unsigned int state = 1806358902;

// Produce the next pseudo-random 32-bit number using xorshift operations.
unsigned int get_random_num(){
  unsigned int num = state;

  num ^= num << 13;
  num ^= num >> 17;
  num ^= num << 5;

  state = num;
  return num;
}

// 64 bit pseoudo legal number

U64 getRandom_U64_nums(){
  U64 n1,n2,n3,n4;


  n1 = (U64)(get_random_num() & 0xFFFF);
  n2 = (U64)(get_random_num() & 0xFFFF);
  n3 = (U64)(get_random_num() & 0xFFFF);
  n4 = (U64)(get_random_num() & 0xFFFF);

  return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);

}


// generate magic number candidate

U64 generate_Magic_Num_Candidate(){
  return (getRandom_U64_nums()&getRandom_U64_nums()&getRandom_U64_nums());
}

// generate magic number

U64 generate_Magic_Num(int square,int relevant_bits,int bishop){
  U64 occupancy[4096];
  U64 attacks[4096];
  U64 used_attacks[4096];

  U64 attack_mask = bishop ? Bishop_Moves::mask_bishop_moves(square) : Rook_Moves::mask_rook_attacks(square);

  U64 occupancy_indices = 1ULL << relevant_bits;

  for(int index = 0;index<occupancy_indices;index++){
    occupancy[index] = set_occupancy(index,relevant_bits,attack_mask);
    attacks[index] = bishop ? Bishop_Moves::bishop_attacks_on_the_fly(square,occupancy[index]) : Rook_Moves::rook_attacks_on_the_fly(square,occupancy[index]);
    // this is where we are leaving vid no : 15 and time as 15:05 **************************************************************
  }

}

int main()
{

  for(int rank = 0;rank<8;rank++){
    for(int file = 0;file<8;file++){
      int sq  =rank*8 + file;
      int sg = count_bits(Bishop_Moves::mask_bishop_moves(sq));

      std::cout << sg << ' ';

    }
    std::cout << '\n';
  }



  std::cout << get_random_num() << std::endl;
  std::cout << get_random_num() << std::endl;
  

  print_Board(getRandom_U64_nums());

	return 0;
}
