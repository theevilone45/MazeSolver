#ifndef INCLUDE_MAZE_COMMON_TYPES_HPP
#define INCLUDE_MAZE_COMMON_TYPES_HPP

#include <array>
#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include <functional>
#include <utility>

namespace std {
    template <>
    struct hash<std::pair<size_t, size_t>> {
        std::size_t operator()(const std::pair<size_t, size_t>& coords) const noexcept {
            std::size_t h1 = std::hash<size_t>{}(coords.first);
            std::size_t h2 = std::hash<size_t>{}(coords.second);
            // Combine the two hashes using bitwise operations.
            return h1 ^ (h2 << 1);
        }
    };
}

namespace maze::common {

using CellCoords = std::pair<size_t, size_t>; // column, row
enum class CellState { NONE, EMPTY, VISITED, PATH, BEGIN, END };
enum class WallDirection { NORTH, EAST, SOUTH, WEST };
using Neighbor = std::tuple<size_t, size_t, WallDirection>;
using NeighborCollection = std::vector<Neighbor>;

struct Walls {
    Walls() { flags.fill(true); }

    inline auto operator[](WallDirection direction) const -> bool {
        return flags[static_cast<size_t>(std::to_underlying(direction))];
    }
    inline auto operator[](WallDirection direction) -> bool & {
        return flags[static_cast<size_t>(std::to_underlying(direction))];
    }

    std::array<bool, 4> flags{};
};

struct Cell {
    size_t column;
    size_t row;
    CellState state;
    Walls walls;
};

struct Maze {
    std::vector<Cell> grid;
    size_t width;
    size_t height;

    Maze(size_t width, size_t height) : width(width), height(height) {}
    inline auto operator[](size_t column, size_t row) -> Cell & {
        return grid.at(row * width + column);
    }
    inline auto operator[](size_t column, size_t row) const -> const Cell & {
        return grid.at(row * width + column);
    }
    inline auto begin() const { return grid.begin(); }
    inline auto end() const { return grid.end(); }
    inline auto begin() { return grid.begin(); }
    inline auto end() { return grid.end(); }
};

} // namespace maze::common

#endif
