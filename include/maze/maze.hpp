#ifndef INCLUDE_MAZE_MAZE_HPP
#define INCLUDE_MAZE_MAZE_HPP

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace maze::logic {

using CellCoords = std::pair<size_t, size_t>; // column, row
enum class CellState { NONE, EMPTY, VISITED, PATH, BEGIN, END };
enum class WallDirection { NORTH, EAST, SOUTH, WEST };

struct Walls {
    Walls() { flags.fill(true); }

    inline auto operator[](WallDirection direction) const -> bool {
        return flags[static_cast<size_t>(std::to_underlying(direction))];
    }
    inline auto operator[](WallDirection direction) -> bool& {
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

auto makeCell(size_t column, size_t row) -> Cell;
auto changeState(Cell &&cell, CellState state) -> Cell;
auto addWall(Cell &&cell, WallDirection direction) -> Cell;
auto removeWall(Cell &&cell, WallDirection direction) -> Cell;

struct Maze {
    std::vector<Cell> grid;
    size_t width;
    size_t height;

    Maze(size_t width, size_t height);
    auto operator[](size_t column, size_t row) -> Cell &;
    auto operator[](size_t column, size_t row) const -> const Cell &;
    inline auto begin() const { return grid.begin(); }
    inline auto end() const { return grid.end(); }
    inline auto begin() { return grid.begin(); }
    inline auto end() { return grid.end(); }
};

auto makeEmptyMaze(size_t width, size_t height) -> Maze;
auto getMazeCellCount(const Maze &maze) -> size_t;
auto generateMaze(Maze &&maze) -> Maze;
auto addBeginCell(Maze &&maze, const CellCoords& coords) -> Maze;
auto addEndCell(Maze &&maze, const CellCoords& coords) -> Maze;

} // namespace maze::logic

#endif
