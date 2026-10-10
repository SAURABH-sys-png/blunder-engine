#pragma once

#include "Position.hpp"

#include <atomic>
#include <cstdint>
#include <functional>

namespace Search {

inline constexpr int MAX_DEPTH = 4;

struct Limits {
	int depth = MAX_DEPTH;
	int time_ms = 0;
};

struct Info {
	int depth = 0;
	int score = 0;
	uint64_t nodes = 0;
	int time_ms = 0;
};

struct Result {
	Move best_move = Move::none();
	int score = 0;
	int depth = 0;
	uint64_t nodes = 0;
};

using InfoCallback = std::function<void(const Info&)>;

Result find_best_move(const Position& position, const Limits& limits,
					  const std::atomic_bool& stop,
					  const InfoCallback& report = {});

} // namespace Search
