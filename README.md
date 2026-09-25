# Blunder Engine

Blunder Engine is a C++17 chess project. The first goal is a simple chess game
that two people can play locally, without AI. A later goal is a playable engine
that can compete near a 1500 Elo rating.

## Current Status

This is an early move-generation prototype, not yet a playable chess game.

- Uses 64-bit bitboards and maps squares from `a8` (index 0) to `h1` (index 63).
- Has attack-pattern code for pawn, knight, king, bishop, and rook movement.
- Bishop and rook attacks include on-the-fly generation and magic-table lookup.
- `main()` currently initializes rook attack tables and prints an empty board and
  the unobstructed rook attacks from `d4`.
- There is no complete board position, starting setup, legal-move list, move
  application, player input, or turn-taking game loop yet.
- There is no search or evaluation function, and no UCI interface for a chess GUI.

As of 2026-09-25, the rough estimate is that 15-20% of the first, human-vs-human
milestone is in place. This is a subjective feature-based estimate, not a
measure of code size. The bot goal is earlier still; Elo strength has not been
measured.

## Roadmap

### Phase 1: Playable Two-Player Game

1. Represent a complete position and initialize the standard starting position.
2. Generate and validate moves, including occupied squares and king safety.
3. Apply moves, captures, and turn changes.
4. Add a local input/output loop so two people can play.
5. Cover the rules supported by the game with tests. For standard chess, this
	includes castling, en passant, promotion, checkmate, stalemate, and draw rules.
6. Verify move generation with attack tests and perft positions.

### Phase 2: Playable Engine

1. Add a searcher, starting with negamax/alpha-beta and iterative deepening.
2. Improve search with move ordering, quiescence search, and a transposition table.
3. Add and tune position evaluation, starting with material and piece-square
	values.
4. Add time management and the UCI protocol so the engine can play in a GUI.
5. Measure strength through games at a defined time control and rating pool, then
	tune based on results.

Near 1500 Elo is an aspiration, not a guarantee. Elo depends on the rating pool,
time control, and testing conditions; it must be measured through matches.

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

The executable currently runs the rook attack demonstration; it is not yet a
human-playable game or a chess bot.
