#include "maze/solvers/bfs.hpp"
#include "maze/utils.hpp"

namespace maze::solvers::bfs {

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

auto BfsSolver::isSolved() const -> bool { return mSolved; }

auto BfsSolver::getNextMazeState(common::Maze &&maze)
    -> monads::Identity<common::Maze> {
    auto mazeMonad = monads::Identity(std::move(maze));

    if(isSolved()) {
        return mazeMonad;
    }

    if (mCellQueue.empty()) {
        return mazeMonad;
    }

    auto current = mCellQueue.front();
    mCellQueue.pop();

    auto neighbors =
        monads::Identity<common::NeighborCollection>()
            .bind(utils::reserveNeighbors, size_t{4})
            .bind(utils::addNeighborIf, mazeMonad.get(),
                  common::WallDirection::NORTH, current, isVisitable)
            .bind(utils::addNeighborIf, mazeMonad.get(),
                  common::WallDirection::EAST, current, isVisitable)
            .bind(utils::addNeighborIf, mazeMonad.get(),
                  common::WallDirection::SOUTH, current, isVisitable)
            .bind(utils::addNeighborIf, mazeMonad.get(),
                  common::WallDirection::WEST, current, isVisitable);

    for (const auto &neighbor : neighbors.get()) {
        const auto [nextColumn, nextRow, direction] = neighbor;
        common::CellCoords nextCell{nextColumn, nextRow};
        mParentMap[nextCell] = current;
        if (mazeMonad.get()[nextColumn, nextRow].state ==
            common::CellState::END) {
            mSolved = true;
            mEnd = nextCell;
            mazeMonad = mazeMonad.bind([this](common::Maze &&maze) {
                return markSolution(std::move(maze));
            });
            return mazeMonad;
        }

        mazeMonad = mazeMonad.bind(utils::changeCellStateInMaze,
                                   common::CellCoords{nextColumn, nextRow},
                                   common::CellState::VISITED);
        mCellQueue.push({nextColumn, nextRow});
    }
    return mazeMonad;
}

auto BfsSolver::markSolution(common::Maze &&maze)
    -> monads::Identity<common::Maze> {
    auto mazeMonad = monads::Identity(std::move(maze));

    std::vector<common::CellCoords> solutionPath;
    common::CellCoords cell = mEnd;

    while (true) {
        cell = mParentMap[cell];
        if(cell == mStart) {
            break;
        }
        solutionPath.push_back(cell);
    }

    for (const auto &pathCell : solutionPath) {
        mazeMonad = mazeMonad.bind(utils::changeCellStateInMaze, pathCell,
                                     common::CellState::PATH);
    }

    return mazeMonad;
}

auto initSolver(const common::CellCoords &begin)
    -> std::unique_ptr<GenericSolver> {
    auto solution = BfsSolver();
    solution.mStart = begin;
    solution.mCellQueue.push(begin);
    return std::make_unique<BfsSolver>(solution);
}

} // namespace maze::solvers::bfs
