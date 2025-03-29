#include "maze/maze.hpp"
#include "maze/debug.hpp"
#include "maze/monads/identity.hpp"
#include "maze/utils.hpp"

#include <cassert>
#include <cstddef>
#include <format>
#include <optional>
#include <print>
#include <random>
#include <utility>
#include <stack>

namespace maze::init {

namespace {

auto addNeighbor(std::vector<common::Neighbor> &&neighbors, const common::Maze &maze,
                 common::WallDirection direction, const common::CellCoords &current)
    -> monads::Identity<std::vector<common::Neighbor>> {
        if (const auto nextCell = utils::getNextCellCoords(current, maze, direction);
        nextCell) {
        const auto [column, row] = *nextCell;
        if (maze[column, row].state == common::CellState::NONE) {
            neighbors.emplace_back(column, row, direction);
        }
    }
    return monads::Identity(std::move(neighbors));
};

auto removeNeighborWalls(common::Maze &&maze, const common::CellCoords &current,
                         const common::CellCoords &neighbor,
                         common::WallDirection direction) -> monads::Identity<common::Maze> {
    auto [column, row] = current;
    auto [nextColumn, nextRow] = neighbor;
    maze[column, row] =
        removeWall(std::move(maze[column, row]), direction).get();
    maze[nextColumn, nextRow] = removeWall(std::move(maze[nextColumn, nextRow]),
                                           utils::getOpositeDirection(direction))
                                    .get();
    return monads::Identity(std::move(maze));
};

}; // namespace

auto makeCell(size_t column, size_t row) -> monads::Identity<common::Cell> {
    return monads::Identity<common::Cell>({.column = column,
                                   .row = row,
                                   .state = common::CellState::NONE,
                                   .walls = common::Walls{}});
}

auto addWall(common::Cell &&cell, common::WallDirection direction) -> monads::Identity<common::Cell> {
    cell.walls[direction] = true;
    return monads::Identity(std::move(cell));
}
auto removeWall(common::Cell &&cell,
                common::WallDirection direction) -> monads::Identity<common::Cell> {
    cell.walls[direction] = false;
    return monads::Identity(std::move(cell));
}

auto makeEmptyMaze(size_t width, size_t height) -> monads::Identity<common::Maze> {
    auto maze = common::Maze(width, height);
    maze.grid.reserve(width * height);
    for (size_t row = 0; row < height; ++row) {
        for (size_t column = 0; column < width; ++column) {
            maze.grid.emplace_back(makeCell(column, row).get());
        }
    }
    LOG(std::format("returning empty maze: {}", maze.grid.size()));
    return monads::Identity(std::move(maze));
}

auto getMazeCellCount(const common::Maze &maze) -> size_t {
    return maze.width * maze.height;
}

auto generateMaze(common::Maze &&maze) -> monads::Identity<common::Maze> {
    std::random_device rd;
    std::mt19937 gen(rd());

    common::CellCoords initCell = {0, 0};
    auto mazeMonad =
        monads::Identity(std::move(maze))
            .bind(utils::changeCellStateInMaze, initCell, common::CellState::EMPTY);

    std::stack<common::CellCoords> cellStack;
    cellStack.push(initCell);

    while (!cellStack.empty()) {
        auto current = cellStack.top();

        // clang-format off
        auto neighbors =
            monads::Identity<std::vector<common::Neighbor>>()
            .bind(utils::reserveNeighbors, size_t{4})
            .bind(addNeighbor, mazeMonad.get(), common::WallDirection::NORTH, current)
            .bind(addNeighbor, mazeMonad.get(), common::WallDirection::EAST,  current)
            .bind(addNeighbor, mazeMonad.get(), common::WallDirection::SOUTH, current)
            .bind(addNeighbor, mazeMonad.get(), common::WallDirection::WEST,  current);

        // neighbors.reserve(4);
        // neighbors =
        //     addNeighbor(
        //         addNeighbor(
        //             addNeighbor(
        //                 addNeighbor(std::move(neighbors),
        //                     maze, common::WallDirection::NORTH, current),
        //                 maze, common::WallDirection::EAST, current),
        //             maze, common::WallDirection::SOUTH, current),
        //         maze, common::WallDirection::WEST, current);

        // clang-format on

        if (neighbors.get().empty()) {
            cellStack.pop();
            continue;
        }

        std::uniform_int_distribution<> dis(
            0, static_cast<int>(neighbors.get().size() - 1));
        const auto [nextColumn, nextRow, direction] =
            neighbors.get()[static_cast<size_t>(dis(gen))];

        mazeMonad =
            mazeMonad
                .bind(removeNeighborWalls, current,
                      common::CellCoords{nextColumn, nextRow}, direction)
                .bind(utils::changeCellStateInMaze, common::CellCoords{nextColumn, nextRow},
                      common::CellState::EMPTY);

        cellStack.push({nextColumn, nextRow});
    }

    return mazeMonad;
}

} // namespace maze::logic
