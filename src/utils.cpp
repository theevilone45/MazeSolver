#include "maze/utils.hpp"
#include "maze/debug.hpp"

#include <cassert>
#include <utility>

namespace maze::utils {

auto isWallPresent(const common::Maze &maze, const common::CellCoords &first,
                   common::WallDirection direction) -> bool {
    const auto [column, row] = first;
    const auto &walls = maze[column, row].walls;
    return walls[direction];
}

auto getOpositeDirection(const common::WallDirection &direction)
    -> common::WallDirection {
    switch (direction) {
    case common::WallDirection::NORTH:
        return common::WallDirection::SOUTH;
    case common::WallDirection::EAST:
        return common::WallDirection::WEST;
    case common::WallDirection::SOUTH:
        return common::WallDirection::NORTH;
    case common::WallDirection::WEST:
        return common::WallDirection::EAST;
    }

    LOG("Fatal error: code should never reach this point");
    assert(false);
    return common::WallDirection::NORTH;
}

auto getNextCellCoords(
    const common::CellCoords &current, const common::Maze &maze,
    common::WallDirection direction) -> std::optional<common::CellCoords> {
    const auto [column, row] = current;
    switch (direction) {
    case common::WallDirection::NORTH:
        if (row > 0) {
            return common::CellCoords{column, row - 1};
        }
        break;
    case common::WallDirection::EAST:
        if (column < maze.width - 1) {
            return common::CellCoords{column + 1, row};
        }
        break;
    case common::WallDirection::SOUTH:
        if (row < maze.height - 1) {
            return common::CellCoords{column, row + 1};
        }
        break;
    case common::WallDirection::WEST:
        if (column > 0) {
            return common::CellCoords{column - 1, row};
        }
        break;
    }
    return std::nullopt;
};

auto reserveNeighbors(std::vector<common::Neighbor> &&neighbors, size_t amount)
    -> monads::Identity<std::vector<common::Neighbor>> {
    neighbors.reserve(amount);
    return monads::Identity(std::move(neighbors));
};

auto addNeighborIf(common::NeighborCollection &&neighbors,
                   const common::Maze &maze, common::WallDirection direction,
                   const common::CellCoords &current, Predicate predicate)
    -> monads::Identity<common::NeighborCollection> {
    const auto next = getNextCellCoords(current, maze, direction);
    if (!next) {
        return monads::Identity{std::move(neighbors)};
    }
    if (predicate(maze, current, *next, direction)) {
        const auto [column, row] = *next;
        neighbors.emplace_back(column, row, direction);
    }
    return monads::Identity{neighbors};
}

auto changeCellStateInMaze(common::Maze &&maze, const common::CellCoords &coords,
                           common::CellState state) -> monads::Identity<common::Maze> {
    const auto [column, row] = coords;
    maze[column, row] =
        monads::Identity(maze[column, row]).bind(changeState, state).get();
    return monads::Identity(std::move(maze));
}

auto changeState(common::Cell &&cell, common::CellState state) -> monads::Identity<common::Cell> {
    cell.state = state;
    return monads::Identity(std::move(cell));
}

} // namespace maze::utils
