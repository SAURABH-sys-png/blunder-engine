#include "Search.hpp"
#include "Position.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <vector>

namespace Search {
namespace {

constexpr int INF = 32000;
constexpr int MATE_SCORE = 30000;
constexpr int MAX_PLY = 96;
constexpr std::array<int, 6> PIECE_VALUES = {100, 320, 335, 500, 900, 0};

struct Context {
    const std::atomic_bool& external_stop;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point deadline;
    bool has_deadline;
    uint64_t nodes = 0;
    bool stopped = false;
};

bool should_stop(Context& context) {
    if (context.external_stop.load(std::memory_order_relaxed)) {
        context.stopped = true;
    } else if (context.has_deadline && std::chrono::steady_clock::now() >= context.deadline) {
        context.stopped = true;
    }
    return context.stopped;
}

int evaluate(const Position& position) {
    int score = 0;
    for (int color = WHITE; color <= BLACK; ++color) {
        int sign = color == WHITE ? 1 : -1;
        for (int piece = PAWN; piece <= KING; ++piece) {
            U64 pieces = position.state[color][piece];
            score += sign * PIECE_VALUES[piece] * __builtin_popcountll(pieces);
            while (pieces) {
                int square = __builtin_ctzll(pieces);
                pieces &= pieces - 1;
                int rank = square / 8;
                int file = square % 8;
                int relative_rank = color == WHITE ? rank : 7 - rank;
                int center_distance = std::abs(3 - file) + std::abs(3 - rank);
                if (piece == PAWN) {
                    int center_file = 3 - std::min(std::abs(3 - file), std::abs(4 - file));
                    score += sign * (relative_rank * 4 + center_file * 5);
                } else if (piece == KNIGHT || piece == BISHOP) {
                    score += sign * (7 - center_distance) * (piece == KNIGHT ? 4 : 2);
                }
            }
        }
    }
    return position.sideToMove == WHITE ? score : -score;
}

int move_order_score(const Position& position, const Move& move) {
    int score = move.is_promotion() ? 8000 + PIECE_VALUES[KNIGHT + ((move.data >> 12) & 3)] : 0;
    if (move.is_capture()) {
        int target = move.to_square();
        int victim_value = 100;
        for (int piece = PAWN; piece <= KING; ++piece) {
            if (position.state[position.sideToMove ^ 1][piece] & (1ULL << target)) {
                victim_value = PIECE_VALUES[piece];
                break;
            }
        }
        int attacker_value = 100;
        for (int piece = PAWN; piece <= KING; ++piece) {
            if (position.state[position.sideToMove][piece] & (1ULL << move.from_square())) {
                attacker_value = PIECE_VALUES[piece];
                break;
            }
        }
        score += 10000 + victim_value * 10 - attacker_value;
    }
    return score;
}

void order_moves(const Position& position, std::vector<Move>& moves) {
    std::sort(moves.begin(), moves.end(), [&position](const Move& left, const Move& right) {
        return move_order_score(position, left) > move_order_score(position, right);
    });
}

bool negamax(Position& position, int depth, int alpha, int beta, int ply,
             Context& context, int& score);

bool quiescence(Position& position, int alpha, int beta, int ply,
                Context& context, int& score) {
    ++context.nodes;
    if (should_stop(context)) return false;

    bool in_check = position.is_in_check(position.sideToMove);
    if (ply >= MAX_PLY) {
        if (!in_check) {
            score = evaluate(position);
            return true;
        }
        std::vector<Move> evasions;
        position.generate_legal_moves(evasions);
        score = evasions.empty() ? -MATE_SCORE + ply : evaluate(position);
        return true;
    }

    int stand_pat = evaluate(position);
    if (!in_check) {
        if (stand_pat >= beta) {
            score = beta;
            return true;
        }
        alpha = std::max(alpha, stand_pat);
        if (ply >= MAX_PLY) {
            score = alpha;
            return true;
        }
    }

    std::vector<Move> moves;
    position.generate_legal_moves(moves);
    if (moves.empty()) {
        score = in_check ? -MATE_SCORE + ply : 0;
        return true;
    }
    order_moves(position, moves);

    for (const Move& move : moves) {
        if (!in_check && !move.is_capture() && !move.is_promotion()) continue;
        Position child = position;
        if (!child.make_move(move)) continue;
        int child_score = 0;
        if (!quiescence(child, -beta, -alpha, ply + 1, context, child_score)) return false;
        int value = -child_score;
        if (value >= beta) {
            score = beta;
            return true;
        }
        alpha = std::max(alpha, value);
    }
    score = alpha;
    return true;
}

bool negamax(Position& position, int depth, int alpha, int beta, int ply,
             Context& context, int& score) {
    ++context.nodes;
    if (should_stop(context)) return false;
    if (depth <= 0) return quiescence(position, alpha, beta, ply, context, score);

    std::vector<Move> moves;
    position.generate_legal_moves(moves);
    if (moves.empty()) {
        score = position.is_in_check(position.sideToMove) ? -MATE_SCORE + ply : 0;
        return true;
    }
    order_moves(position, moves);

    int best_score = -INF;
    for (const Move& move : moves) {
        Position child = position;
        if (!child.make_move(move)) continue;
        int child_score = 0;
        if (!negamax(child, depth - 1, -beta, -alpha, ply + 1, context, child_score)) return false;
        int value = -child_score;
        best_score = std::max(best_score, value);
        alpha = std::max(alpha, value);
        if (alpha >= beta) break;
    }
    score = best_score;
    return true;
}

} // namespace

Result find_best_move(const Position& position, const Limits& limits,
                      const std::atomic_bool& stop, const InfoCallback& report) {
    Result result;
    Position root_position = position;
    std::vector<Move> legal_moves;
    root_position.generate_legal_moves(legal_moves);
    if (legal_moves.empty()) return result;
    order_moves(root_position, legal_moves);
    result.best_move = legal_moves.front();

    Context context{stop, std::chrono::steady_clock::now(), {}, limits.time_ms > 0};
    if (context.has_deadline) {
        context.deadline = context.start + std::chrono::milliseconds(limits.time_ms);
    }
    int max_depth = std::clamp(limits.depth, 1, MAX_DEPTH);
    for (int depth = 1; depth <= max_depth; ++depth) {
        if (should_stop(context)) break;
        Move iteration_best = result.best_move;
        int iteration_score = -INF;
        int alpha = -INF;
        for (const Move& move : legal_moves) {
            Position child = root_position;
            if (!child.make_move(move)) continue;
            int child_score = 0;
            if (!negamax(child, depth - 1, -INF, -alpha, 1, context, child_score)) break;
            int value = -child_score;
            if (value > iteration_score) {
                iteration_score = value;
                iteration_best = move;
            }
            alpha = std::max(alpha, value);
        }
        if (context.stopped) break;

        result.best_move = iteration_best;
        result.score = iteration_score;
        result.depth = depth;
        result.nodes = context.nodes;
        if (report) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - context.start).count();
            report(Info{depth, iteration_score, context.nodes, static_cast<int>(elapsed)});
        }
        if (std::abs(iteration_score) >= MATE_SCORE - MAX_PLY) break;
    }
    result.nodes = context.nodes;
    return result;
}

} // namespace Search