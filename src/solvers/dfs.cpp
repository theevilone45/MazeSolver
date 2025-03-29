#include "maze/solvers/dfs.hpp"
#include "maze/common/types.hpp"
#include "maze/debug.hpp"
#include "maze/monads/identity.hpp"
#include "maze/solvers/generic_solver.hpp"
#include "maze/utils.hpp"
#include <cstddef>
#include <memory>

namespace maze::solvers::dfs {

namespace {

auto isVisitable(const common::Maze &maze, const common::CellCoords &current,
                 const common::CellCoords &next,
                 common::WallDirection direction) -> bool {
    const auto [nextColumn, nextRow] = next;

    if (maze[nextColumn, nextRow].state != common::CellState::EMPTY &&
        maze[nextColumn, nextRow].state != common::CellState::END) {
        return false;
    }
    const auto opositeDirection = utils::getOpositeDirection(direction);
    if (!utils::isWallPresent(maze, current, direction) &&
        !utils::isWallPresent(maze, next, opositeDirection)) {
        return true;
    }
    return false;
}

} // namespace

auto DfsSolver::isSolved() const -> bool { return mSolved; }

auto DfsSolver::getNextMazeState(common::Maze &&maze)
    -> monads::Identity<common::Maze> {
    LOG("started solving maze with DFS");
    LOG(std::format("cellStack size: {}", mCellStack.size()));

    auto mazeMonad = monads::Identity{std::move(maze)};

    if (isSolved()) {
        return mazeMonad;
    }

    if (mCellStack.empty()) {
        LOG("cell stack is empty");
        return mazeMonad;
    }

    auto current = mCellStack.top();

    // clang-format off
    auto neighbors = monads::Identity<common::NeighborCollection>()
        .bind(utils::reserveNeighbors, size_t{4})
        .bind(utils::addNeighborIf, mazeMonad.get(), common::WallDirection::NORTH, current , isVisitable)
        .bind(utils::addNeighborIf, mazeMonad.get(), common::WallDirection::EAST, current , isVisitable)
        .bind(utils::addNeighborIf, mazeMonad.get(), common::WallDirection::SOUTH, current , isVisitable)
        .bind(utils::addNeighborIf, mazeMonad.get(), common::WallDirection::WEST, current , isVisitable);

    // clang-format on

    if (neighbors.get().empty()) {
        mCellStack.pop();
        LOG("neighbors are empty");
        return mazeMonad;
    }

    const auto [nextColumn, nextRow, direction] =
        neighbors.get()[static_cast<size_t>(0)];

    if (mazeMonad.get()[nextColumn, nextRow].state == common::CellState::END) {
        mSolved = true;
        mazeMonad = mazeMonad.bind(
            [this](common::Maze &&maze) { return markSolution(std::move(maze)); });
        return mazeMonad;
    }

    mazeMonad = mazeMonad.bind(utils::changeCellStateInMaze,
                               common::CellCoords{nextColumn, nextRow},
                               common::CellState::VISITED);
    LOG("cell visited");
    mCellStack.push({nextColumn, nextRow});
    return mazeMonad;
}

auto DfsSolver::markSolution(common::Maze &&maze)
    -> monads::Identity<common::Maze> {
    auto mazeMonad = monads::Identity{std::move(maze)};
    while (!mCellStack.empty()) {
        const auto pathCell = mCellStack.top();
        mazeMonad = mazeMonad.bind(utils::changeCellStateInMaze, pathCell,
                                   common::CellState::PATH);
        mCellStack.pop();
    }
    return mazeMonad;
}

auto initSolver(const common::CellCoords &begin) -> std::unique_ptr<GenericSolver> {
    auto solution = DfsSolver();
    solution.mCellStack.push(begin);
    return std::make_unique<DfsSolver>(solution);
}

} // namespace maze::solvers::dfs
