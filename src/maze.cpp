#include "maze/maze.hpp"
#include "maze/monads/identity.hpp"

#include <cassert>
#include <cstddef>
#include <optional>
#include <print>
#include <random>
#include <stack>
#include <vector>

namespace maze::logic {

using Neighbor = std::tuple<size_t, size_t, WallDirection>;

namespace {

auto getNextCellCoords(const CellCoords &current, const Maze &maze,
                       WallDirection direction) -> std::optional<CellCoords> {
    const auto [column, row] = current;
    switch (direction) {
    case WallDirection::NORTH:
        if (row > 0) {
            return CellCoords{column, row - 1};
        }
        break;
    case WallDirection::EAST:
        if (column < maze.width - 1) {
            return CellCoords{column + 1, row};
        }
        break;
    case WallDirection::SOUTH:
        if (row < maze.height - 1) {
            return CellCoords{column, row + 1};
        }
        break;
    case WallDirection::WEST:
        if (column > 0) {
            return CellCoords{column - 1, row};
        }
        break;
    }
    return std::nullopt;
};

auto getOpositeDirection(const WallDirection &direction) -> WallDirection {
    switch (direction) {
    case WallDirection::NORTH:
        return WallDirection::SOUTH;
    case WallDirection::EAST:
        return WallDirection::WEST;
    case WallDirection::SOUTH:
        return WallDirection::NORTH;
    case WallDirection::WEST:
        return WallDirection::EAST;
    }
    assert(false && "this should never occur");
    return WallDirection::NORTH;
}

const auto addNeighbor = [](std::vector<Neighbor> &&neighbors,
                            const Maze &maze, WallDirection direction,
                            const CellCoords &current)
    -> monads::Identity<std::vector<Neighbor>> {
    if (const auto nextCell = getNextCellCoords(current, maze, direction);
        nextCell) {
        const auto [column, row] = *nextCell;
        if (maze[column, row].state == CellState::NONE) {
            neighbors.emplace_back(column, row, direction);
        }
    }
    return monads::Identity(std::move(neighbors));
};

auto removeNeighborWalls(Maze &&maze, const CellCoords &current, const CellCoords &neighbor,
       WallDirection direction) -> monads::Identity<Maze> {
    auto [column, row] = current;
    auto [nextColumn, nextRow] = neighbor;
    maze[column, row] =
        removeWall(std::move(maze[column, row]), direction).get();
    maze[nextColumn, nextRow] =
        removeWall(std::move(maze[nextColumn, nextRow]),
                   getOpositeDirection(direction))
            .get();
    return monads::Identity(std::move(maze));
};

const auto reserveVec =
    [](std::vector<Neighbor> &&neighbors,
       size_t amount) -> monads::Identity<std::vector<Neighbor>> {
    neighbors.reserve(amount);
    return monads::Identity(std::move(neighbors));
};

}; // namespace

auto makeCell(size_t column, size_t row) -> monads::Identity<Cell> {
    return monads::Identity<Cell>({.column = column,
                                   .row = row,
                                   .state = CellState::NONE,
                                   .walls = Walls{}});
}

auto changeState(Cell &&cell, CellState state) -> monads::Identity<Cell> {
    cell.state = state;
    return monads::Identity(std::move(cell));
}
auto addWall(Cell &&cell, WallDirection direction) -> monads::Identity<Cell> {
    cell.walls[direction] = true;
    return monads::Identity(std::move(cell));
}
auto removeWall(Cell &&cell,
                WallDirection direction) -> monads::Identity<Cell> {
    cell.walls[direction] = false;
    return monads::Identity(std::move(cell));
}

Maze::Maze(size_t width, size_t height) : width(width), height(height) {}

auto Maze::operator[](size_t column, size_t row) -> Cell & {
    return grid.at(row * width + column);
}
auto Maze::operator[](size_t column, size_t row) const -> const Cell & {
    return grid.at(row * width + column);
}

auto makeEmptyMaze(size_t width, size_t height) -> monads::Identity<Maze> {
    auto maze = Maze(width, height);
    maze.grid.reserve(width * height);
    for (size_t row = 0; row < height; ++row) {
        for (size_t column = 0; column < width; ++column) {
            maze.grid.emplace_back(makeCell(column, row).get());
        }
    }
    return monads::Identity(std::move(maze));
}

auto getMazeCellCount(const Maze &maze) -> size_t {
    return maze.width * maze.height;
}

auto generateMaze(Maze &&maze) -> monads::Identity<Maze> {
    std::random_device rd;
    std::mt19937 gen(rd());

    maze[0, 0].state = CellState::EMPTY;

    std::stack<CellCoords> cellStack;
    cellStack.push({0, 0});

    while (!cellStack.empty()) {
        auto current = cellStack.top();

        // clang-format off

        auto neighbors =
            monads::Identity<std::vector<Neighbor>>()
            .bind(reserveVec, 4)
            .bind(addNeighbor, maze, WallDirection::NORTH, current)
            .bind(addNeighbor, maze, WallDirection::EAST,  current)
            .bind(addNeighbor, maze, WallDirection::SOUTH, current)
            .bind(addNeighbor, maze, WallDirection::WEST,  current);

        // neighbors.reserve(4);
        // neighbors =
        //     addNeighbor(
        //         addNeighbor(
        //             addNeighbor(
        //                 addNeighbor(std::move(neighbors),
        //                     maze, WallDirection::NORTH, current),
        //                 maze, WallDirection::EAST, current),
        //             maze, WallDirection::SOUTH, current),
        //         maze, WallDirection::WEST, current);
        // clang-format on

        if (neighbors.get().empty()) {
            cellStack.pop();
            continue;
        }

        std::uniform_int_distribution<> dis(
            0, static_cast<int>(neighbors.get().size() - 1));
        const auto [nextColumn, nextRow, direction] =
            neighbors.get()[static_cast<size_t>(dis(gen))];

        auto mazeMonad = monads::Identity(std::move(maze))
            .bind(removeNeighborWalls, current, {nextColumn, nextRow}, direction);


        maze[nextColumn, nextRow] =
            changeState(std::move(maze[nextColumn, nextRow]), CellState::EMPTY);

        cellStack.push({nextColumn, nextRow});
    }

    return monads::Identity(std::move(maze));
}

// auto addBeginCell(Maze &&maze, const CellCoords &coords) -> monads::Identity<Maze> {
//     const auto [column, row] = coords;
//     maze[column, row] = monads::Identity(maze[column, row])
//         .bind(changeState, CellState::BEGIN).get();
//     return monads::Identity(std::move(maze));
// }
// auto addEndCell(Maze &&maze, const CellCoords &coords) -> monads::Identity<Maze> {
//     const auto [column, row] = coords;
//     maze[column, row] = monads::Identity(maze[column, row])
//         .bind(changeState, CellState::END).get();
//     return monads::Identity(std::move(maze));
// }

} // namespace maze::logic
