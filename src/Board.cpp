#include "Board.hpp"

// Creates a board from the given state and generates all moves.
Board::Board(const Brd &board, const Color &self)
    : board_(board),
      slf_(self),
      all_moves_(8, std::vector<Moves>(8))
{
    generateMoves();
}

// Generates moves for every square on the board.
void Board::generateMoves()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            all_moves_[i][j] = each_Piece_moves(i, j);
        }
    }
}

// Returns all possible moves for the piece at (i, j).
Moves Board::each_Piece_moves(int i, int j)
{
    if (slf_ != board_[i][j].clr)
        return {};

    switch (board_[i][j].piece)
    {
    case Pieces::Pawn:
        return get_Pawn_Moves(i, j, slf_);

    case Pieces::Rook:
        return get_Rook_Moves(i, j, slf_);

    case Pieces::King:
        return get_King_Moves(i, j, slf_);

    case Pieces::Queen:
        return get_Queen_Moves(i, j, slf_);

    case Pieces::Bishop:
        return get_Bishop_Moves(i, j, slf_);

    case Pieces::Knight:
        return get_Knight_Moves(i, j, slf_);

    default:
        return {};
    }
}

// Generates possible pawn moves.
Moves Board::get_Pawn_Moves(int i, int j, Color clr)
{
    Moves moves;

    // Implement pawn move logic here.

    return moves;
}

// Generates possible rook moves.
Moves Board::get_Rook_Moves(int i, int j, Color clr)
{
    Moves moves;

    // Implement rook move logic here.

    return moves;
}

// Generates possible king moves.
Moves Board::get_King_Moves(int i, int j, Color clr)
{
    Moves moves;

    // Implement king move logic here.

    return moves;
}

// Generates possible queen moves.
Moves Board::get_Queen_Moves(int i, int j, Color clr)
{
    Moves moves;

    // Implement queen move logic here.

    return moves;
}

// Generates possible bishop moves.
Moves Board::get_Bishop_Moves(int i, int j, Color clr)
{
    Moves moves;

    // Implement bishop move logic here.

    return moves;
}

// Generates possible knight moves.
Moves Board::get_Knight_Moves(int i, int j, Color clr)
{
    Moves moves;

    // Implement knight move logic here.

    return moves;
}