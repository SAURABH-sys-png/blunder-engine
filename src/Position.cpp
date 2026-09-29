#include "Position.hpp"
#include "Rook_Moves.hpp"
#include "Bishop_Moves.hpp"

#include <iostream>
#include <sstream>
#include <cctype>
#include <string>

// Builtin bit-scan intrinsics (GCC/Clang)
static inline int get_lsb(U64 bb) {
    return __builtin_ctzll(bb);
}

static inline U64 pop_lsb(U64& bb) {
    int lsb = get_lsb(bb);
    bb &= bb - 1;
    return lsb;
}

// Precomputed attack tables for non-sliding pieces
static U64 knight_attacks[64];
static U64 king_attacks[64];
static U64 pawn_attacks[2][64];

static void init_leaper_attacks() {
    static bool initialized = false;
    if (initialized) return;

    for (int sq = 0; sq < 64; ++sq) {
        int r = sq / 8;
        int f = sq % 8;

        // Knight attacks
        U64 k_mask = 0ULL;
        const int dr_k[] = {-2, -2, -1, -1, 1, 1, 2, 2};
        const int df_k[] = {-1,  1, -2,  2, -2, 2, -1, 1};
        for (int i = 0; i < 8; ++i) {
            int nr = r + dr_k[i], nf = f + df_k[i];
            if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
                k_mask |= (1ULL << (nr * 8 + nf));
            }
        }
        knight_attacks[sq] = k_mask;

        // King attacks
        U64 king_mask = 0ULL;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int df = -1; df <= 1; ++df) {
                if (dr == 0 && df == 0) continue;
                int nr = r + dr, nf = f + df;
                if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8) {
                    king_mask |= (1ULL << (nr * 8 + nf));
                }
            }
        }
        king_attacks[sq] = king_mask;

        // Pawn attacks
        // White pawns attack up-left and up-right (rank + 1)
        U64 wp_mask = 0ULL;
        if (r < 7) {
            if (f > 0) wp_mask |= (1ULL << ((r + 1) * 8 + (f - 1)));
            if (f < 7) wp_mask |= (1ULL << ((r + 1) * 8 + (f + 1)));
        }
        pawn_attacks[WHITE][sq] = wp_mask;

        // Black pawns attack down-left and down-right (rank - 1)
        U64 bp_mask = 0ULL;
        if (r > 0) {
            if (f > 0) bp_mask |= (1ULL << ((r - 1) * 8 + (f - 1)));
            if (f < 7) bp_mask |= (1ULL << ((r - 1) * 8 + (f + 1)));
        }
        pawn_attacks[BLACK][sq] = bp_mask;
    }
    initialized = true;
}

// Castling rights update table indexed by square
// Clears castling if rooks or kings move/get captured
static constexpr int castling_rights_update[64] = {
    13, 15, 15, 15, 12, 15, 15, 14, // rank 1: a1=13 (~WQ), e1=12 (~WK & ~WQ), h1=14 (~WK)
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11  // rank 8: a8=7 (~BQ), e8=3 (~BK & ~BQ), h8=11 (~BK)
};

Position::Position() {
    init_leaper_attacks();
    resetEverything();
}

void Position::resetEverything() {
    for (int c = 0; c < 2; ++c) {
        for (int p = 0; p < 6; ++p) {
            state[c][p] = 0ULL;
        }
    }
    occupancies[WHITE_OC] = 0ULL;
    occupancies[BLACK_OC] = 0ULL;
    occupancies[BOTH_OC] = 0ULL;

    sideToMove = WHITE;
    enPassantSquare = NO_SQ;
    castlingRights = 0;
    halfMoveClock = 0;
    fullMoveNumber = 1;
}

void Position::updateOccupancies() {
    occupancies[WHITE_OC] = state[WHITE][PAWN] | state[WHITE][KNIGHT] |
                            state[WHITE][BISHOP] | state[WHITE][ROOK] |
                            state[WHITE][QUEEN] | state[WHITE][KING];

    occupancies[BLACK_OC] = state[BLACK][PAWN] | state[BLACK][KNIGHT] |
                            state[BLACK][BISHOP] | state[BLACK][ROOK] |
                            state[BLACK][QUEEN] | state[BLACK][KING];

    occupancies[BOTH_OC] = occupancies[WHITE_OC] | occupancies[BLACK_OC];
}

void Position::set_startpos() {
    set_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Position::set_from_fen(std::string fen) {
    resetEverything();
    std::istringstream iss{std::string(fen)};
    std::string piece_placement, active_color, castling, ep_square;
    int halfmove = 0, fullmove = 1;

    iss >> piece_placement >> active_color >> castling >> ep_square >> halfmove >> fullmove;

    // 1. Piece placement
    int rank = 7;
    int file = 0;
    for (char ch : piece_placement) {
        if (ch == '/') {
            rank--;
            file = 0;
        } else if (std::isdigit(ch)) {
            file += (ch - '0');
        } else {
            int sq = rank * 8 + file;
            int color = std::isupper(ch) ? WHITE : BLACK;
            char lower = std::tolower(ch);
            int piece_type = PAWN;

            switch (lower) {
                case 'p': piece_type = PAWN; break;
                case 'n': piece_type = KNIGHT; break;
                case 'b': piece_type = BISHOP; break;
                case 'r': piece_type = ROOK; break;
                case 'q': piece_type = QUEEN; break;
                case 'k': piece_type = KING; break;
            }

            state[color][piece_type] |= (1ULL << sq);
            file++;
        }
    }

    // 2. Active color
    sideToMove = (active_color == "b") ? BLACK : WHITE;

    // 3. Castling rights
    castlingRights = 0;
    for (char ch : castling) {
        if (ch == 'K') castlingRights |= WK;
        else if (ch == 'Q') castlingRights |= WQ;
        else if (ch == 'k') castlingRights |= BK;
        else if (ch == 'q') castlingRights |= BQ;
    }

    // 4. En-passant square
    if (ep_square != "-" && ep_square.length() >= 2) {
        int ep_file = ep_square[0] - 'a';
        int ep_rank = ep_square[1] - '1';
        enPassantSquare = ep_rank * 8 + ep_file;
    } else {
        enPassantSquare = NO_SQ;
    }

    halfMoveClock = halfmove;
    fullMoveNumber = fullmove;

    updateOccupancies();
}

bool Position::is_square_attacked(int sq, int attacking_color) const {
    U64 occ = occupancies[BOTH_OC];

    // Attacked by pawns: reverse test with opposite side's attack mask
    int defending_color = attacking_color ^ 1;
    if (pawn_attacks[defending_color][sq] & state[attacking_color][PAWN]) {
        return true;
    }

    // Attacked by knights
    if (knight_attacks[sq] & state[attacking_color][KNIGHT]) {
        return true;
    }

    // Attacked by king
    if (king_attacks[sq] & state[attacking_color][KING]) {
        return true;
    }

    // Attacked by bishops or queens
    U64 bishop_like = state[attacking_color][BISHOP] | state[attacking_color][QUEEN];
    if (Bishop_Moves::get_bishop_attacks(sq, occ) & bishop_like) {
        return true;
    }

    // Attacked by rooks or queens
    U64 rook_like = state[attacking_color][ROOK] | state[attacking_color][QUEEN];
    if (Rook_Moves::get_rook_attacks(sq, occ) & rook_like) {
        return true;
    }

    return false;
}

bool Position::is_in_check(int color) const {
    U64 king_bb = state[color][KING];
    if (!king_bb) return false;
    int king_sq = get_lsb(king_bb);
    return is_square_attacked(king_sq, color ^ 1);
}

bool Position::make_move(const Move& m) {
    int from = m.from_square();
    int to = m.to_square();
    int us = sideToMove;
    int them = us ^ 1;

    if (!m.is_valid() || from == to) return false;
    U64 from_mask = 1ULL << from;
    U64 to_mask = 1ULL << to;
    if ((occupancies[us] & to_mask) || (state[them][KING] & to_mask)) return false;

    // Identify moving piece
    int moved_piece = -1;
    for (int p = 0; p < 6; ++p) {
        if (state[us][p] & from_mask) {
            moved_piece = p;
            break;
        }
    }
    if (moved_piece == -1) return false;

    Position previous = *this;

    // Handle captures
    for (int p = 0; p < 6; ++p) {
        if (state[them][p] & to_mask) {
            state[them][p] &= ~to_mask;
            break;
        }
    }

    // Move piece from -> to
    state[us][moved_piece] &= ~from_mask;
    state[us][moved_piece] |= to_mask;

    // En-passant capture
    if (moved_piece == PAWN && to == enPassantSquare) {
        int ep_captured_sq = (us == WHITE) ? (to - 8) : (to + 8);
        state[them][PAWN] &= ~(1ULL << ep_captured_sq);
    }

    // Promotions
    if (m.is_promotion()) {
        state[us][PAWN] &= ~(1ULL << to);
        int promo_type = (m.data >> 12) & 0x3; // 0=N, 1=B, 2=R, 3=Q
        int actual_promo_piece = KNIGHT + promo_type;
        state[us][actual_promo_piece] |= (1ULL << to);
    }

    // Castling moves
    if (moved_piece == KING) {
        if (from == 4 && to == 6) { // White King-side
            state[WHITE][ROOK] &= ~(1ULL << 7);
            state[WHITE][ROOK] |= (1ULL << 5);
        } else if (from == 4 && to == 2) { // White Queen-side
            state[WHITE][ROOK] &= ~(1ULL << 0);
            state[WHITE][ROOK] |= (1ULL << 3);
        } else if (from == 60 && to == 62) { // Black King-side
            state[BLACK][ROOK] &= ~(1ULL << 63);
            state[BLACK][ROOK] |= (1ULL << 61);
        } else if (from == 60 && to == 58) { // Black Queen-side
            state[BLACK][ROOK] &= ~(1ULL << 56);
            state[BLACK][ROOK] |= (1ULL << 59);
        }
    }

    // Update en-passant square
    if (moved_piece == PAWN && std::abs(to - from) == 16) {
        enPassantSquare = (from + to) / 2;
    } else {
        enPassantSquare = NO_SQ;
    }

    // Update castling rights
    castlingRights &= castling_rights_update[from];
    castlingRights &= castling_rights_update[to];

    updateOccupancies();

    // Verify move legality (cannot leave king in check)
    if (is_in_check(us)) {
        *this = previous;
        return false;
    }

    // Advance turn
    sideToMove = them;
    if (us == BLACK) fullMoveNumber++;

    return true;
}

void Position::generate_pseudo_legal_moves(std::vector<Move>& move_list) {
    int us = sideToMove;
    int them = us ^ 1;
    U64 occ_all = occupancies[BOTH_OC];
    U64 empty = ~occ_all;
    U64 enemy_or_empty = ~(occupancies[us] | state[them][KING]);

    // 1. Pawn moves
    U64 pawns = state[us][PAWN];
    while (pawns) {
        int sq = pop_lsb(pawns);
        int rank = sq / 8;

        // Single push
        int single_to = (us == WHITE) ? (sq + 8) : (sq - 8);
        if (single_to >= 0 && single_to < 64 && !(occ_all & (1ULL << single_to))) {
            bool is_promo = (us == WHITE && rank == 6) || (us == BLACK && rank == 1);
            if (is_promo) {
                for (int pt = 0; pt < 4; ++pt) move_list.emplace_back(sq, single_to, pt, false);
            } else {
                move_list.emplace_back(sq, single_to);
                // Double push from home rank
                int double_to = (us == WHITE) ? (sq + 16) : (sq - 16);
                if (((us == WHITE && rank == 1) || (us == BLACK && rank == 6)) && !(occ_all & (1ULL << double_to))) {
                    move_list.emplace_back(sq, double_to);
                }
            }
        }

        // Pawn attacks
        U64 attacks = pawn_attacks[us][sq] & ((occupancies[them] & ~state[them][KING]) | (enPassantSquare != NO_SQ ? (1ULL << enPassantSquare) : 0ULL));
        while (attacks) {
            int to = pop_lsb(attacks);
            bool is_promo = (us == WHITE && rank == 6) || (us == BLACK && rank == 1);
            if (is_promo) {
                for (int pt = 0; pt < 4; ++pt) move_list.emplace_back(sq, to, pt, true);
            } else {
                move_list.emplace_back(sq, to, -1, true);
            }
        }
    }

    // 2. Knights
    U64 knights = state[us][KNIGHT];
    while (knights) {
        int sq = pop_lsb(knights);
        U64 moves = knight_attacks[sq] & enemy_or_empty;
        while (moves) {
            int to = pop_lsb(moves);
            move_list.emplace_back(sq, to, -1, (occupancies[them] & (1ULL << to)) != 0);
        }
    }

    // 3. Bishops & Queens (diagonals)
    U64 bishops = state[us][BISHOP] | state[us][QUEEN];
    while (bishops) {
        int sq = pop_lsb(bishops);
        U64 moves = Bishop_Moves::get_bishop_attacks(sq, occ_all) & enemy_or_empty;
        while (moves) {
            int to = pop_lsb(moves);
            move_list.emplace_back(sq, to, -1, (occupancies[them] & (1ULL << to)) != 0);
        }
    }

    // 4. Rooks & Queens (orthogonals)
    U64 rooks = state[us][ROOK] | state[us][QUEEN];
    while (rooks) {
        int sq = pop_lsb(rooks);
        U64 moves = Rook_Moves::get_rook_attacks(sq, occ_all) & enemy_or_empty;
        while (moves) {
            int to = pop_lsb(moves);
            move_list.emplace_back(sq, to, -1, (occupancies[them] & (1ULL << to)) != 0);
        }
    }

    // 5. King
    U64 king = state[us][KING];
    if (king) {
        int sq = get_lsb(king);
        U64 moves = king_attacks[sq] & enemy_or_empty;
        while (moves) {
            int to = pop_lsb(moves);
            move_list.emplace_back(sq, to, -1, (occupancies[them] & (1ULL << to)) != 0);
        }

        // Castling checks (squares must be empty and unattacked)
        if (us == WHITE && sq == 4 && !is_in_check(WHITE)) {
            if ((castlingRights & WK) && (state[WHITE][ROOK] & (1ULL << 7)) && !(occ_all & ((1ULL << 5) | (1ULL << 6)))) {
                if (!is_square_attacked(5, BLACK) && !is_square_attacked(6, BLACK)) {
                    move_list.emplace_back(4, 6);
                }
            }
            if ((castlingRights & WQ) && (state[WHITE][ROOK] & (1ULL << 0)) && !(occ_all & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3)))) {
                if (!is_square_attacked(3, BLACK) && !is_square_attacked(2, BLACK)) {
                    move_list.emplace_back(4, 2);
                }
            }
        } else if (us == BLACK && sq == 60 && !is_in_check(BLACK)) {
            if ((castlingRights & BK) && (state[BLACK][ROOK] & (1ULL << 63)) && !(occ_all & ((1ULL << 61) | (1ULL << 62)))) {
                if (!is_square_attacked(61, WHITE) && !is_square_attacked(62, WHITE)) {
                    move_list.emplace_back(60, 62);
                }
            }
            if ((castlingRights & BQ) && (state[BLACK][ROOK] & (1ULL << 56)) && !(occ_all & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59)))) {
                if (!is_square_attacked(59, WHITE) && !is_square_attacked(58, WHITE)) {
                    move_list.emplace_back(60, 58);
                }
            }
        }
    }
}

void Position::generate_legal_moves(std::vector<Move>& move_list) {
    std::vector<Move> pseudo_moves;
    generate_pseudo_legal_moves(pseudo_moves);

    for (const auto& m : pseudo_moves) {
        Position copy = *this;
        if (copy.make_move(m)) {
            move_list.push_back(m);
        }
    }
}

void Position::print_board() const {
    std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << " | ";
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            char piece = '.';

            if (state[WHITE][PAWN] & (1ULL << sq)) piece = 'P';
            else if (state[WHITE][KNIGHT] & (1ULL << sq)) piece = 'N';
            else if (state[WHITE][BISHOP] & (1ULL << sq)) piece = 'B';
            else if (state[WHITE][ROOK] & (1ULL << sq)) piece = 'R';
            else if (state[WHITE][QUEEN] & (1ULL << sq)) piece = 'Q';
            else if (state[WHITE][KING] & (1ULL << sq)) piece = 'K';
            else if (state[BLACK][PAWN] & (1ULL << sq)) piece = 'p';
            else if (state[BLACK][KNIGHT] & (1ULL << sq)) piece = 'n';
            else if (state[BLACK][BISHOP] & (1ULL << sq)) piece = 'b';
            else if (state[BLACK][ROOK] & (1ULL << sq)) piece = 'r';
            else if (state[BLACK][QUEEN] & (1ULL << sq)) piece = 'q';
            else if (state[BLACK][KING] & (1ULL << sq)) piece = 'k';

            std::cout << piece << " | ";
        }
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "    a   b   c   d   e   f   g   h\n\n";
}