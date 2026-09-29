#include "Position.hpp"
#include <cmath>

// Precalculated lookup array for updating castling rights branchlessly.
// Clears relevant rights if a king or rook moves, or if a rook square is captured.
constexpr int castling_rights_mask[64] = {
    13, 15, 15, 15, 12, 15, 15, 14, // Rank 1 (a1=13 [~WQ], e1=12 [~(WK|WQ)], h1=14 [~WK])
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11  // Rank 8 (a8=7 [~BQ],  e8=3 [~(BK|BQ)],  h8=11 [~BK])
};

Position::Position()
{
    resetEverything();
}

void Position::resetEverything()
{
    for (int c = WHITE; c <= BLACK; ++c) {
        for (int p = PAWN; p <= KING; ++p) {
            state[c][p] = 0ULL;
        }
    }
    
    occupancies[WHITE_OC] = 0ULL;
    occupancies[BLACK_OC] = 0ULL;
    occupancies[BOTH_OC]  = 0ULL;

    sideToMove = WHITE;
    enPassantSquare = 64;
    castlingRights = WK | WQ | BK | BQ;
    halfMoveClock = 0;
    fullMoveNumber = 1;
}

void Position::updateOccupencies()
{
    occupancies[WHITE_OC] = state[WHITE][PAWN] | state[WHITE][KNIGHT] | state[WHITE][BISHOP] |
                            state[WHITE][ROOK] | state[WHITE][QUEEN]  | state[WHITE][KING];
    occupancies[BLACK_OC] = state[BLACK][PAWN] | state[BLACK][KNIGHT] | state[BLACK][BISHOP] |
                            state[BLACK][ROOK] | state[BLACK][QUEEN]  | state[BLACK][KING];
    occupancies[BOTH_OC]  = occupancies[WHITE_OC] | occupancies[BLACK_OC];
}

void Position::MakeMove(std::string_view moveStr)
{
    // 1. Fast coordinate parsing (e.g., "e2e4", "e7e8q")
    int fromSq = (moveStr[0] - 'a') + (moveStr[1] - '1') * 8;
    int toSq   = (moveStr[2] - 'a') + (moveStr[3] - '1') * 8;

    U64 fromBB = 1ULL << fromSq;
    U64 toBB   = 1ULL << toSq;
    U64 moveBB = fromBB | toBB; // XOR mask to move piece from source to dest

    int us   = sideToMove;
    int them = sideToMove ^ 1;

    // 2. Identify moving piece
    int movedPiece = PAWN;
    for (int p = PAWN; p <= KING; ++p) {
        if (state[us][p] & fromBB) {
            movedPiece = p;
            break;
        }
    }

    // 3. Clear captured piece (if any)
    int capturedPiece = -1;
    if (occupancies[them] & toBB) {
        for (int p = PAWN; p <= KING; ++p) {
            if (state[them][p] & toBB) {
                capturedPiece = p;
                state[them][p] ^= toBB;
                break;
            }
        }
    }

    // 4. Move piece / Handle Promotion
    bool isPromotion = (moveStr.length() >= 5 && moveStr[4] != '\0');
    if (isPromotion) {
        state[us][PAWN] ^= fromBB; // Remove pawn

        int promoPiece = QUEEN;
        switch (moveStr[4]) {
            case 'n': case 'N': promoPiece = KNIGHT; break;
            case 'b': case 'B': promoPiece = BISHOP; break;
            case 'r': case 'R': promoPiece = ROOK;   break;
            case 'q': case 'Q': promoPiece = QUEEN;  break;
        }
        state[us][promoPiece] |= toBB; // Place promoted piece
    } else {
        state[us][movedPiece] ^= moveBB; // Single XOR toggles both squares
    }

    // 5. Handle En Passant target square & En Passant captures
    int newEnPassantSquare = 64; // Default no_sq
    if (movedPiece == PAWN) {
        if (std::abs(toSq - fromSq) == 16) {
            newEnPassantSquare = (fromSq + toSq) >> 1; // Double push
        } else if (toSq == enPassantSquare) {
            // En Passant capture: remove target pawn behind destination square
            int epTargetSq = toSq + (us == WHITE ? -8 : 8);
            state[them][PAWN] ^= (1ULL << epTargetSq);
        }
    }
    enPassantSquare = newEnPassantSquare;

    // 6. Handle Castling rook movements
    if (movedPiece == KING) {
        if (fromSq == 4 && toSq == 6) {         // White Kingside (e1g1)
            state[WHITE][ROOK] ^= (1ULL << 7) | (1ULL << 5);
        } else if (fromSq == 4 && toSq == 2) {  // White Queenside (e1c1)
            state[WHITE][ROOK] ^= (1ULL << 0) | (1ULL << 3);
        } else if (fromSq == 60 && toSq == 62) { // Black Kingside (e8g8)
            state[BLACK][ROOK] ^= (1ULL << 63) | (1ULL << 61);
        } else if (fromSq == 60 && toSq == 58) { // Black Queenside (e8c8)
            state[BLACK][ROOK] ^= (1ULL << 56) | (1ULL << 59);
        }
    }

    // 7. Update Castling Rights branchlessly
    castlingRights &= castling_rights_mask[fromSq] & castling_rights_mask[toSq];

    // 8. Update Move Counters
    if (movedPiece == PAWN || capturedPiece != -1) {
        halfMoveClock = 0;
    } else {
        halfMoveClock++;
    }

    if (us == BLACK) {
        fullMoveNumber++;
    }

    // 9. Switch Turn & Rebuild Occupancies
    sideToMove ^= 1;
    updateOccupencies();
}