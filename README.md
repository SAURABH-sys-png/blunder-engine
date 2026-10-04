# Blunder Engine

Blunder Engine is an experimental C++17 bitboard chess engine with a UCI
interface. It can be loaded into a UCI-compatible chess GUI or driven from the
command line.

## Current Status

The engine can search positions and return legal moves through UCI. Its playing
strength is preliminary and has not been measured.

- Uses 64-bit bitboards and maps squares from `a8` (index 0) to `h1` (index 63).
- Has attack-pattern code for pawn, knight, king, bishop, and rook movement.
- Bishop and rook attacks include on-the-fly generation and magic-table lookup.
- Implements iterative-deepening negamax with alpha-beta pruning, quiescence
	search, basic material/positional evaluation, and capture ordering.
- Supports UCI `position`, depth and clock-limited `go`, `stop`, readiness, and
	search info output.
- Repetition handling, a transposition table, and comprehensive chess-rule
	tests are not implemented yet.

Near 1500 Elo remains an aspiration, not a guarantee. Elo depends on the rating
pool, time control, and testing conditions; it must be measured through matches.

## Roadmap

### Phase 1: Playable Two-Player Game

1. Represent a complete position and initialize the standard starting position.
2. Generate and validate moves, including occupied squares and king safety.
3. Apply moves, captures, and turn changes.
4. Add a local input/output loop so two people can play.
5. Cover the rules supported by the game with tests. For standard chess, this
	includes castling, en passant, promotion, checkmate, stalemate, and draw rules.
6. Verify move generation with attack tests and perft positions.

### Engine Strength

1. Add repetition and draw handling, a transposition table, and stronger move
	ordering.
2. Add focused move-generation and perft tests for supported chess rules.
3. Measure strength through games at a defined time control and rating pool, then
	tune based on results.

## Recent Fixes

- Magic-number validation checks collisions at the computed magic-table index.
- The candidate failure flag is initialized and remains in scope for validation.
- Rook magic numbers are generated using the rook attack path.
- The rook attack table is initialized before its test lookup in `main()`.

## Build and Run

From the repository root:

```sh
cmake -S . -B build
cmake --build build
./build/blunder-engine
```

The executable speaks UCI on standard input/output. For an interactive terminal
search, start the engine and enter:

```sh
./build/blunder-engine
uci
isready
position startpos
go depth 5
```

For interactive play, add `build/blunder-engine` as a UCI engine in a chess
GUI. The engine is functional, but its playing strength has not been measured.
