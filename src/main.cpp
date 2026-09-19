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

// magic bishop & rook matrices
U64 rook_magic_number[64] = {
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
U64 bishop_magic_number[64] = {
  11575658452893700ULL, 7077010911297920ULL, 72218176692682840ULL, 18094662864601088ULL,
  438925656096ULL, 3603442876281831424ULL, 867365214016765984ULL, 594477351715213314ULL,
  13186539709279520842ULL, 4364273160ULL, 2309858580834033676ULL, 13549350508560896ULL,
  5765742220208505856ULL, 4620746269523181696ULL, 1154973201904697600ULL, 10423689402123264ULL,
  387344758976053264ULL, 163822909654433792ULL, 146649014769549312ULL, 2308094844477636657ULL,
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

  for(int random_count = 0;random_count < 100000000;random_count++){
    U64 magic_number = generate_Magic_Num_Candidate();
    if(count_bits((attack_mask * magic_number)&0xFF00000000000000) < 6)continue;
    // init used attacks
    memset(used_attacks,0ULL,sizeof(used_attacks));
    // test magix index
    // init index ad fail flag
    int index,fail;

    for(int index = 0,fail = 0;!fail && index < occupancy_indices;index++){
      // init magic index
      int magic_index = (int)((occupancy[index] * magic_number) >> (64 - relevant_bits));
      // on empty index
      if(used_attacks[index] == 0ULL){
        // magic index works
        used_attacks[index] = attacks[index];
      }
      else if(used_attacks[index] != attacks[index])
      {
        // magic index doesnt work
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

// init magic number

void init_magic_numbers(){
  for(int sq = 0;sq<64;sq++){
    // init rook magic number
    rook_magic_number[sq] = generate_Magic_Num(sq,rook_relevant_bits[sq],1);
  }
  printf("\n\n");


  for(int sq = 0;sq<64;sq++){
    // init bishop magic number
    bishop_magic_number[sq] = generate_Magic_Num(sq,bishop_relevant_bits[sq],1);
  }
}



int main()
{
  
  init_magic_numbers();
  // cotinue from vid number 16

	return 0;
}
