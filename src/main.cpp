#include <bits/stdc++.h>
#include "Position.hpp"
// Include the move-generation interfaces used by this test program.
#include "Bishop_Moves.hpp"
#include "Rook_Moves.hpp"
#include "Pawn_Moves.hpp"
// Use a short name for the 64-bit bitboard type.
#define U64 unsigned long long

// File masks used to prevent attacks from wrapping around board edges.
const U64 not_A_file = 18374403900871474942ULL;
const U64 not_H_file = 9187201950435737471ULL;
const U64 not_HG_file = 4557430888798830399ULL;
const U64 not_AB_file = 18229723555140126207ULL;
// Map each board square to an index from a8 (0) through h1 (63).
enum
{
  a8,
  b8,
  c8,
  d8,
  e8,
  f8,
  g8,
  h8,
  a7,
  b7,
  c7,
  d7,
  e7,
  f7,
  g7,
  h7,
  a6,
  b6,
  c6,
  d6,
  e6,
  f6,
  g6,
  h6,
  a5,
  b5,
  c5,
  d5,
  e5,
  f5,
  g5,
  h5,
  a4,
  b4,
  c4,
  d4,
  e4,
  f4,
  g4,
  h4,
  a3,
  b3,
  c3,
  d3,
  e3,
  f3,
  g3,
  h3,
  a2,
  b2,
  c2,
  d2,
  e2,
  f2,
  g2,
  h2,
  a1,
  b1,
  c1,
  d1,
  e1,
  f1,
  g1,
  h1
};

const char *sq_to_coordinates[64] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};
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

// Helper function to setup the standard starting chess position on a Position object
void setup_start_position(Position &pos)
{
  pos.resetEverything();

  // White pieces (a1 = 0, h1 = 7, a2 = 8, h2 = 15)
  pos.state[WHITE][PAWN] = 0x000000000000FF00ULL;       // Rank 2
  pos.state[WHITE][KNIGHT] = (1ULL << 1) | (1ULL << 6); // b1, g1
  pos.state[WHITE][BISHOP] = (1ULL << 2) | (1ULL << 5); // c1, f1
  pos.state[WHITE][ROOK] = (1ULL << 0) | (1ULL << 7);   // a1, h1
  pos.state[WHITE][QUEEN] = (1ULL << 3);                // d1
  pos.state[WHITE][KING] = (1ULL << 4);                 // e1

  // Black pieces (a7 = 48, h7 = 55, a8 = 56, h8 = 63)
  pos.state[BLACK][PAWN] = 0x00FF000000000000ULL;         // Rank 7
  pos.state[BLACK][KNIGHT] = (1ULL << 57) | (1ULL << 62); // b8, g8
  pos.state[BLACK][BISHOP] = (1ULL << 58) | (1ULL << 61); // c8, f8
  pos.state[BLACK][ROOK] = (1ULL << 56) | (1ULL << 63);   // a8, h8
  pos.state[BLACK][QUEEN] = (1ULL << 59);                 // d8
  pos.state[BLACK][KING] = (1ULL << 60);                  // e8

  pos.updateOccupencies();
}

// Helper function to display full board state graphically with piece letters
void print_position(const Position &pos)
{
  const char piece_chars[2][6] = {
      {'P', 'N', 'B', 'R', 'Q', 'K'},
      {'p', 'n', 'b', 'r', 'q', 'k'}};

  std::cout << "    +---+---+---+---+---+---+---+---+\n";
  for (int rank = 7; rank >= 0; rank--)
  {
    std::cout << "  " << (rank + 1) << " |";
    for (int file = 0; file < 8; file++)
    {
      int sq = rank * 8 + file;
      char pieceChar = '.';

      for (int color = WHITE; color <= BLACK; ++color)
      {
        for (int piece = PAWN; piece <= KING; ++piece)
        {
          if (pos.state[color][piece] & (1ULL << sq))
          {
            pieceChar = piece_chars[color][piece];
            break;
          }
        }
      }
      std::cout << " " << pieceChar << " |";
    }
    std::cout << "\n    +---+---+---+---+---+---+---+---+\n";
  }
  std::cout << "      a   b   c   d   e   f   g   h\n\n";

  std::string epSqStr = "none";
  if (pos.enPassantSquare < 64)
  {
    char f = 'a' + (pos.enPassantSquare % 8);
    char r = '1' + (pos.enPassantSquare / 8);
    epSqStr = {f, r};
  }

  std::cout << "Side to move    : " << (pos.sideToMove == WHITE ? "WHITE" : "BLACK") << "\n";
  std::cout << "En Passant sq   : " << epSqStr << "\n";
  std::cout << "Castling Rights : " << pos.castlingRights << " (Mask)\n";
  std::cout << "Halfmove Clock  : " << pos.halfMoveClock << "\n";
  std::cout << "Fullmove Number : " << pos.fullMoveNumber << "\n";
  std::cout << "---------------------------------------------\n\n";
}

int main()
{
  // 1. Initialize lookup tables
  Pawn_Moves::init_pawn_attacks();

  std::cout << "========================================================\n";
  std::cout << "  1. TESTING RAW PAWN ATTACK MASKS\n";
  std::cout << "========================================================\n";

  std::cout << "White Pawn Attacks on e2:";
  print_Board(Pawn_Moves::get_pawn_attacks(e2, WHITE));

  std::cout << "Black Pawn Attacks on e7:";
  print_Board(Pawn_Moves::get_pawn_attacks(e7, BLACK));

  std::cout << "========================================================\n";
  std::cout << "  2. TESTING QUIET PUSHES (SINGLE & DOUBLE PUSH)\n";
  std::cout << "========================================================\n";

  U64 empty_board = 0ULL;
  std::cout << "White Pawn on e2 (Starting rank, empty board -> e3 & e4 expected):";
  print_Board(Pawn_Moves::get_pawn_pushes(e2, WHITE, empty_board));

  // Test double push blocking: place a piece on e4
  U64 occupied_e4 = (1ULL << e4);
  std::cout << "White Pawn on e2 with obstacle on e4 -> (e3 only expected):";
  print_Board(Pawn_Moves::get_pawn_pushes(e2, WHITE, occupied_e4));

  // Test total blocking: place a piece on e3
  U64 occupied_e3 = (1ULL << e3);
  std::cout << "White Pawn on e2 with obstacle on e3 -> (No moves expected):";
  print_Board(Pawn_Moves::get_pawn_pushes(e2, WHITE, occupied_e3));

  std::cout << "========================================================\n";
  std::cout << "  3. TESTING CAPTURES & EN PASSANT\n";
  std::cout << "========================================================\n";

  // Place enemy pieces on d5 and f5 for a White Pawn on e4
  U64 enemies = (1ULL << d5) | (1ULL << f5);
  int no_ep = 64;

  std::cout << "White Pawn on e4 with Black pieces on d5 & f5:";
  print_Board(Pawn_Moves::get_pawn_captures(e4, WHITE, enemies, no_ep));

  // Test En Passant Capture
  // White pawn on e5 (sq 28), Black double-pushed to d5 (sq 27), setting EP target square to d6 (sq 19)
  int ep_square_d6 = d6;
  std::cout << "White Pawn on e5 with En Passant target on d6:";
  print_Board(Pawn_Moves::get_pawn_captures(e5, WHITE, 0ULL, ep_square_d6));

  std::cout << "========================================================\n";
  std::cout << "  4. TESTING COMBINED MOVESET (PUSHES + CAPTURES)\n";
  std::cout << "========================================================\n";

  // White Pawn on e2, enemy on d3, block on e4 (so push to e3 only + capture on d3)
  U64 test_both_occupancy = (1ULL << e4) | (1ULL << d3);
  U64 test_enemy_occupancy = (1ULL << d3);

  std::cout << "Combined moves for White Pawn on e2 (Enemy d3, Blocked e4):";
  print_Board(Pawn_Moves::get_pawn_moves(e2, WHITE, test_both_occupancy, test_enemy_occupancy, no_ep));

  return 0;
}
