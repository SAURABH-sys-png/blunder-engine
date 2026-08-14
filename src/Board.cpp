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

    if (clr == Color::White)
    {
        // Three types of moves
        // 1. Move forward by one square.
        if (i < 7 && board_[i - 1][j].piece == Pieces::None)
        {
            moves.push_back({i + 1, j});
        }
        // 2. Move forward by two squares from the starting position.
        if (i == 6 && board_[i - 1][j].piece == Pieces::None && board_[i - 2][j].piece == Pieces::None)
        {
            moves.push_back({i - 2, j});
        }
        // 3. Capture diagonally.
        // Leave it for now, as it requires more context about the board state and opponent pieces.
    }
    else if (clr == Color::Black)
    {
        // Three types of moves
        // 1. Move forward by one square.
        if (i > 0 && board_[i + 1][j].piece == Pieces::None)
        {
            moves.push_back({i + 1, j});
        }
        // 2. Move forward by two squares from the starting position.
        if (i == 1 && board_[i + 1][j].piece == Pieces::None && board_[i + 2][j].piece == Pieces::None)
        {
            moves.push_back({i + 2, j});
        }
        // 3. Capture diagonally.
        // Leave it for now, as it requires more context about the board state and opponent pieces.
    }

    return moves;
}

// Generates possible rook moves.
Moves Board::get_Rook_Moves(int i, int j, Color clr)
{
    Moves moves;

    //
    for (int row = i - 1; row >= 0; --row)
    {
        if (board_[row][j].piece == Pieces::None)
        {
            moves.push_back({row, j});
        }
        else
        {
            if (board_[row][j].clr != clr)
            {
                moves.push_back({row, j});
            }
            break;
        }
    }
    for (int row = i + 1; row < 8; ++row)
    {
        if (board_[row][j].piece == Pieces::None)
        {
            moves.push_back({row, j});
        }
        else
        {
            if (board_[row][j].clr != clr)
            {
                moves.push_back({row, j});
            }
            break;
        }
    }
    for (int col = j - 1; col >= 0; --col)
    {
        if (board_[i][col].piece == Pieces::None)
        {
            moves.push_back({i, col});
        }
        else
        {
            if (board_[i][col].clr != clr)
            {
                moves.push_back({i, col});
            }
            break;
        }
    }
    for (int col = j + 1; col < 8; ++col)
    {
        if (board_[i][col].piece == Pieces::None)
        {
            moves.push_back({i, col});
        }
        else
        {
            if (board_[i][col].clr != clr)
            {
                moves.push_back({i, col});
            }
            break;
        }
    }

    return moves;
}

// Generates possible king moves.
Moves Board::get_King_Moves(int i, int j, Color clr)
{
    Moves moves;

    

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