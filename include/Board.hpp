#pragma once

#include <array>
#include <vector>

// Represents the type of piece on a board square.
enum class Pieces
{
    Pawn,
    Rook,
    King,
    Queen,
    Bishop,
    Knight,
    None
};

// Represents the color/side of a piece.
enum class Color
{
    White,
    Black,
    None
};

// Represents a piece and its color on a board square.
struct Piece
{
    Pieces piece = Pieces::None;
    Color clr = Color::None;
};

// Represents a position on the board using row and column.
struct Position
{
    int row;
    int col;
};

// Stores all possible moves for a single piece.
using Moves = std::vector<Position>;

// Stores possible moves for every piece on the board.
using All_Moves = std::vector<std::vector<Moves>>;

// Represents the complete 8x8 current state of the chess board.
using Brd = std::array<std::array<Piece, 8>, 8>;

// Handles board state and chess move generation.
class Board
{
private:
    // Current state of the chess board.
    Brd board_;

    // Color for which moves are currently being generated.
    Color slf_;

    // Stores generated moves for pieces on the board.
    All_Moves all_moves_;

    // Generates possible moves for a pawn at (i, j).
    Moves get_Pawn_Moves(int i, int j, Color clr);

    // Generates possible moves for a rook at (i, j).
    Moves get_Rook_Moves(int i, int j, Color clr);

    // Generates possible moves for a king at (i, j).
    Moves get_King_Moves(int i, int j, Color clr);

    // Generates possible moves for a queen at (i, j).
    Moves get_Queen_Moves(int i, int j, Color clr);

    // Generates possible moves for a bishop at (i, j).
    Moves get_Bishop_Moves(int i, int j, Color clr);

    // Generates possible moves for a knight at (i, j).
    Moves get_Knight_Moves(int i, int j, Color clr);

public:
    // Creates a board using the given board state and side.
    Board(const Brd &board, const Color &self);

    // Generates moves for all pieces belonging to the current side.
    void generateMoves();

    // Generates moves for the piece located at (i, j).
    Moves each_Piece_moves(int i, int j);
};